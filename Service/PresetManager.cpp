/*
  ==============================================================================

    PresetManager.cpp
    Created: 16 Mar 2025 10:12:14am
    Author:  kyleb

  ==============================================================================
*/

#include "PresetManager.h"


namespace Service
{
    const juce::File PresetManager::defaultDirectory
    {
        juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
        .getChildFile(ProjectInfo::companyName)
        .getChildFile(ProjectInfo::projectName)
    };

    const juce::String PresetManager::extension{"preset"};

    PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
        : apvts(apvts)
    {
        // create a default preset directory 
        if (!defaultDirectory.exists())
        {
            const auto result = defaultDirectory.createDirectory();
            if (result.failed())
            {
                DBG("Could not create preset directory: " + result.getErrorMessage());
                jassertfalse;
            }

        }
    }

    void PresetManager::savePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
        {
            return;
        }

        const std::unique_ptr<juce::XmlElement> xml = apvts.copyState().createXml();
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);

        if (!xml->writeTo(presetFile))
        {
            DBG("Could not create this preset file");
            jassertfalse;
        }

        currentPreset = presetName;
    }

    void PresetManager::deletePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
        {
            return;
        }

        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (presetFile.exists())
        {
            DBG("Preset File: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }
        if (!presetFile.deleteFile())
        {
            DBG("Could not delete Preset File: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }

        currentPreset = "";
    }

    void PresetManager::loadPreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
        {
            return;
        }
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset File: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }

        juce::XmlDocument xmlDocument{ presetFile };
        const juce::ValueTree apvtsToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

        apvts.replaceState(apvtsToLoad);
        currentPreset = presetName;
    }


    void PresetManager::loadNextPreset()
    {
        const auto allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return;

        const int currentIndex = allPresets.indexOf(currentPreset);
        const int nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;

        loadPreset(allPresets.getReference(nextIndex));
    }

    void PresetManager::loadPreviousPreset()
    {
        const auto allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return;

        const int currentIndex = allPresets.indexOf(currentPreset);
        const int previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;

        loadPreset(allPresets.getReference(previousIndex));
    }

    juce::StringArray PresetManager::getAllPresets() const
    {
        juce::StringArray presetList;
        const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);

        for (const auto& file : fileArray)
        {
            presetList.add(file.getFileNameWithoutExtension());
        }
        return presetList;
    }

    juce::String PresetManager::getCurrentPreset() const
    {
        return currentPreset;
    }
} // namespace Service
