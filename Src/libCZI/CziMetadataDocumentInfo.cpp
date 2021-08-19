//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#include "stdafx.h"
#include "CziMetadataDocumentInfo.h"
#include "CziDimensionInfo.h"
#include "CziDisplaySettings.h"
#include "utilities.h"
#include "CziMetadataDocumentInfo2.h"

using namespace libCZI;
using namespace std;

CCziMetadataDocumentInfo::CCziMetadataDocumentInfo(std::shared_ptr<CCziMetadata> md)
    : metadata(md)
{
    this->ParseDimensionInfo();
}

/*virtual*/libCZI::GeneralDocumentInfo CCziMetadataDocumentInfo::GetGeneralDocumentInfo() const
{
    GeneralDocumentInfo generalDocumentInfo;
    auto np = this->GetNode(L"Metadata/Information/Document");
    if (!np.empty())
    {
        auto n = GetNodeRelativeFromNode(np, L"UserName");
        if (!n.empty())
        {
            generalDocumentInfo.userName = n.text().as_string();
            generalDocumentInfo.userName_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Title");
        if (!n.empty())
        {
            generalDocumentInfo.title = n.text().as_string();
            generalDocumentInfo.title_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Description");
        if (!n.empty())
        {
            generalDocumentInfo.description = n.text().as_string();
            generalDocumentInfo.description_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Comment");
        if (!n.empty())
        {
            generalDocumentInfo.comment = n.text().as_string();
            generalDocumentInfo.comment_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Rating");
        if (!n.empty())
        {
            generalDocumentInfo.rating = n.text().as_int();
            generalDocumentInfo.rating_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"CreationDate");
        if (!n.empty())
        {
            generalDocumentInfo.creationDateTime = n.text().as_string();
            generalDocumentInfo.creationDateTime_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Keywords");
        if (!n.empty())
        {
            generalDocumentInfo.keywords = n.text().as_string();
            generalDocumentInfo.keywords_valid = true;
        }

        n = GetNodeRelativeFromNode(np, L"Name");
        if (!n.empty())
        {
            generalDocumentInfo.name = n.text().as_string();
            generalDocumentInfo.name_valid = true;
        }
    }

    return generalDocumentInfo;
}

/*virtual*/libCZI::ScalingInfoEx CCziMetadataDocumentInfo::GetScalingInfoEx() const
{
    ScalingInfoEx scalingInfo;
    auto np = this->GetNode(L"Metadata/Scaling");
    if (np.empty())
    {
        return scalingInfo;
    }

    static const struct
    {
        char	dimChar;
        double(ScalingInfoEx::* scaleVarPtr);
        std::wstring(ScalingInfoEx::* defaultUnit);
    } dimScalingData[] =
    {
        {'X', &ScalingInfoEx::scaleX, &ScalingInfoEx::defaultUnitFormatX},
        {'Y', &ScalingInfoEx::scaleY, &ScalingInfoEx::defaultUnitFormatY},
        {'Z', &ScalingInfoEx::scaleZ, &ScalingInfoEx::defaultUnitFormatZ},
    };

    for (const auto d : dimScalingData)
    {
        wstringstream ss;
        ss << L"Items/Distance[@Id='" << d.dimChar << L"']/Value";
        auto nodeScalingValue = np.select_node(ss.str().c_str());
        if (!nodeScalingValue.node().empty())
        {
            scalingInfo.*d.scaleVarPtr = nodeScalingValue.node().text().as_double();
        }

        ss = wstringstream();
        ss << L"Items/Distance[@Id='" << d.dimChar << L"']/DefaultUnitFormat";
        auto nodeScalingDefaultUnit = np.select_node(ss.str().c_str());
        if (!nodeScalingDefaultUnit.node().empty())
        {
            scalingInfo.*d.defaultUnit = nodeScalingDefaultUnit.node().text().as_string();
        }
    }

    return scalingInfo;
}

/*virtual*/libCZI::ScalingInfo CCziMetadataDocumentInfo::GetScalingInfo() const
{
    ScalingInfo scalingInfo;
    auto np = this->GetNode(L"Metadata/Scaling");
    if (np.empty())
    {
        return scalingInfo;
    }

    static const struct
    {
        char	dimChar;
        double(ScalingInfo::* scaleVarPtr);
    } dimScalingData[] =
    {
        {'X', &ScalingInfoEx::scaleX},
        {'Y', &ScalingInfoEx::scaleY},
        {'Z', &ScalingInfoEx::scaleZ},
    };

    for (const auto d : dimScalingData)
    {
        wstringstream ss;
        ss << L"Items/Distance[@Id='" << d.dimChar << L"']/Value";
        auto nodeScalingValue = np.select_node(ss.str().c_str());
        if (!nodeScalingValue.node().empty())
        {
            scalingInfo.*d.scaleVarPtr = nodeScalingValue.node().text().as_double();
        }
    }

    return scalingInfo;
}

/*virtual*/void CCziMetadataDocumentInfo::EnumDimensions(std::function<bool(libCZI::DimensionIndex)> enumDimensions)
{
    for (const auto& it : this->dimensions)
    {
        if (enumDimensions(it.first) == false)
        {
            break;
        }
    }
}

/*virtual*/std::shared_ptr<libCZI::IDimensionInfo> CCziMetadataDocumentInfo::GetDimensionInfo(libCZI::DimensionIndex dim)
{
    const auto it = this->dimensions.find(dim);
    if (it == this->dimensions.cend())
    {
        // TODO: or should we throw an exception?
        return std::shared_ptr<libCZI::IDimensionInfo>();
    }

    switch (dim)
    {
    case DimensionIndex::C:
    case DimensionIndex::T:
    case DimensionIndex::Z:
    case DimensionIndex::B:

        // TODO: not sure about these... should we report them here...?
    case DimensionIndex::R:
    case DimensionIndex::S:
    case DimensionIndex::I:
    case DimensionIndex::H:
    case DimensionIndex::V:
        return std::make_shared<CCziDimensionInfo>(this->metadata, it->second);
    default:
        throw std::logic_error("unknown dimension, not implemented");
    }
}

/*virtual*/std::shared_ptr<IDimensionZInfo> CCziMetadataDocumentInfo::GetDimensionZInfo()
{
    auto dimZnode = this->GetNode(L"Metadata/Information/Image/Dimensions/Z");
    if (!dimZnode)
    {
        return std::shared_ptr<IDimensionZInfo>();
    }

    auto zinfo = make_shared<CCziDimensionZInfo>();

    auto startPosNode = GetNodeRelativeFromNode(dimZnode, L"StartPosition");
    if (!startPosNode.empty())
    {
        double d = startPosNode.text().as_double();
        zinfo->SetStartPosition(d);
    }

    auto xyzHandednessNode = GetNodeRelativeFromNode(dimZnode, L"XYZHandedness");
    if (!xyzHandednessNode.empty())
    {
        auto s = Utilities::Trim(xyzHandednessNode.text().as_string());
        if (s.compare(L"LeftHanded") == 0)
        {
            zinfo->SetXyzHandedness(IDimensionZInfo::XyzHandedness::LeftHanded);
        }
        else if (s.compare(L"RightHanded") == 0)
        {
            zinfo->SetXyzHandedness(IDimensionZInfo::XyzHandedness::RightHanded);
        }
        else if (s.compare(L"undefined") == 0)
        {
            zinfo->SetXyzHandedness(IDimensionZInfo::XyzHandedness::Undefined);
        }
    }

    auto zaxisDirNode = GetNodeRelativeFromNode(dimZnode, L"ZAxisDirection");
    if (!zaxisDirNode.empty())
    {
        auto s = Utilities::Trim(zaxisDirNode.text().as_string());
        if (s.compare(L"FromSpecimenToObjective") == 0)
        {
            zinfo->SetZAxisDirection(IDimensionZInfo::ZaxisDirection::FromSpecimenToObjective);
        }
        else if (s.compare(L"FromObjectiveToSpecimen") == 0)
        {
            zinfo->SetZAxisDirection(IDimensionZInfo::ZaxisDirection::FromObjectiveToSpecimen);
        }
        else if (s.compare(L"undefined") == 0)
        {
            zinfo->SetZAxisDirection(IDimensionZInfo::ZaxisDirection::Undefined);
        }
    }

    auto zdriveModeNode = GetNodeRelativeFromNode(dimZnode, L"ZDriveMode");
    if (!zdriveModeNode.empty())
    {
        auto s = Utilities::Trim(zaxisDirNode.text().as_string());
        if (s.compare(L"Continuous") == 0)
        {
            zinfo->SetZDriveMode(IDimensionZInfo::ZDriveMode::Continuous);
        }
        else if (s.compare(L"Step") == 0)
        {
            zinfo->SetZDriveMode(IDimensionZInfo::ZDriveMode::Step);
        }
    }

    auto zdriveSpeedNode = GetNodeRelativeFromNode(dimZnode, L"ZDriveSpeed");
    if (!zdriveSpeedNode.empty())
    {
        auto str = Utilities::Trim(zdriveSpeedNode.text().as_string());
        if (!str.empty())
        {
            size_t charsParsed; double x;
            bool parsedOk = false;
            try
            {
                x = std::stod(str, &charsParsed);
                parsedOk = true;
            }
            catch (invalid_argument&)
            {
            }

            if (parsedOk == true)
            {
                if (charsParsed < str.length() && !isspace(str[charsParsed]))
                {
                    parsedOk = false;
                }
            }

            if (parsedOk)
            {
                zinfo->SetZDriveSpeed(x);
            }
        }
    }

    auto intervalNode = GetNodeRelativeFromNode(dimZnode, L"Positions/Interval");
    if (!!intervalNode)
    {
        double start = 0, increment = 0;
        auto n = GetNodeRelativeFromNode(intervalNode, L"Start");
        if (!n.empty())
        {
            start = n.text().as_double();
        }

        n = GetNodeRelativeFromNode(intervalNode, L"Increment");
        if (!n.empty())
        {
            increment = n.text().as_double();
        }

        zinfo->SetIntervalDefinition(start, increment);
    }
    else
    {
        auto offsetsNode = GetNodeRelativeFromNode(dimZnode, L"Positions/List/Offsets");
        if (!!offsetsNode)
        {
            vector<double> data;
            Utilities::Split(offsetsNode.text().as_string(), L' ',
                [&](const std::wstring str)->bool
                {
                    size_t charsParsed;
                    bool parsedOk = false;
                    double x;
                    try
                    {
                        x = std::stod(str, &charsParsed);
                        parsedOk = true;
                    }
                    catch (invalid_argument&)
                    {
                    }

                    if (parsedOk == true)
                    {
                        if (charsParsed < str.length() && !isspace(str[charsParsed]))
                        {
                            parsedOk = false;
                        }
                    }

                    // TODO: currently, we stop parsing at the first syntax error and return what we have so far without an 
                    //        external error - what is the desired behavior here?
                    if (parsedOk == true)
                    {
                        data.push_back(x);
                    }

                    return parsedOk;
                });

            zinfo->SetListDefinition(std::move(data));
        }
    }

    return zinfo;
}

/*virtual*/std::shared_ptr<IDimensionTInfo> CCziMetadataDocumentInfo::GetDimensionTInfo()
{
    auto dimTnode = this->GetNode(L"Metadata/Information/Image/Dimensions/T");
    if (!dimTnode)
    {
        return std::shared_ptr<IDimensionTInfo>();
    }

    auto tinfo = make_shared<CCziDimensionTInfo>();
    auto startPosNode = GetNodeRelativeFromNode(dimTnode, L"StartTime");
    if (!startPosNode.empty())
    {
        auto s = startPosNode.text().as_string();
        XmlDateTime dt;
        if (XmlDateTime::TryParse(s, &dt))
        {
            tinfo->SetStartTime(dt);
        }
    }

    auto intervalNode = GetNodeRelativeFromNode(dimTnode, L"Positions/Interval");
    if (!!intervalNode)
    {
        double start = 0, increment = 0;
        auto n = GetNodeRelativeFromNode(intervalNode, L"Start");
        if (!n.empty())
        {
            start = n.text().as_double();
        }

        n = GetNodeRelativeFromNode(intervalNode, L"Increment");
        if (!n.empty())
        {
            increment = n.text().as_double();
        }

        tinfo->SetIntervalDefinition(start, increment);
    }
    else
    {
        auto offsetsNode = GetNodeRelativeFromNode(dimTnode, L"Positions/List/Offsets");
        if (!!offsetsNode)
        {
            vector<double> data;
            Utilities::Split(offsetsNode.text().as_string(), L' ',
                [&](const std::wstring str)->bool
                {
                    size_t charsParsed;
                    bool parsedOk = false;
                    double x;
                    try
                    {
                        x = std::stod(str, &charsParsed);
                        parsedOk = true;
                    }
                    catch (invalid_argument&)
                    {
                    }

                    if (parsedOk == true)
                    {
                        if (charsParsed < str.length() && !isspace(str[charsParsed]))
                        {
                            parsedOk = false;
                        }
                    }

                    // TODO: currently, we stop parsing at the first syntax error and return what we have so far without an 
                    //        external error - what is the desired behavior here?
                    if (parsedOk == true)
                    {
                        data.push_back(x);
                    }

                    return parsedOk;
                });

            tinfo->SetListDefinition(std::move(data));
        }
    }

    return tinfo;
}

/*virtual*/std::shared_ptr<IDimensionsChannelsInfo> CCziMetadataDocumentInfo::GetDimensionChannelsInfo()
{
    return CDimensionsChannelsInfo::TryParse(this->metadata.get());
}

/*virtual*/std::shared_ptr<libCZI::IDisplaySettings> CCziMetadataDocumentInfo::GetDisplaySettings() const
{
    auto dsplSetting = this->GetNode(L"Metadata/DisplaySetting");
    if (dsplSetting.empty())
    {
        // TODO - exception or empty object?
        return std::shared_ptr<libCZI::IDisplaySettings>();
    }

    return CDisplaySettingsOnPod::CreateFromXml(dsplSetting);
}

void CCziMetadataDocumentInfo::ParseDimensionInfo()
{
    static const struct
    {
        DimensionIndex index;
        const wchar_t* start, * size;
    } DimAndNodeNames[] =
    {
        { DimensionIndex::Z,L"StartZ",L"SizeZ"},
        { DimensionIndex::C,L"StartC",L"SizeC" },
        { DimensionIndex::T,L"StartT",L"SizeT" },
        { DimensionIndex::R,L"StartR",L"SizeR" },
        { DimensionIndex::S,L"StartS",L"SizeS" },
        { DimensionIndex::I,L"StartI",L"SizeI" },
        { DimensionIndex::H,L"StartH",L"SizeH" },
        { DimensionIndex::V,L"StartV",L"SizeV" },
        { DimensionIndex::B,L"StartB",L"SizeB" }
    };

    auto np = this->GetNode(L"Metadata/Information/Image");
    if (!np.empty())
    {
        for (int i = 0; i < sizeof(DimAndNodeNames) / sizeof(DimAndNodeNames[0]); ++i)
        {
            auto nStart = GetNodeRelativeFromNode(np, DimAndNodeNames[i].start);
            auto nSize = GetNodeRelativeFromNode(np, DimAndNodeNames[i].size);
            if (!nStart.empty() || !nSize.empty())
            {
                int start = 0, size = 1;
                if (!nStart.empty())
                {
                    start = nStart.text().as_int();
                }

                if (!nSize.empty())
                {
                    size = nSize.text().as_int();
                }

                this->dimensions[DimAndNodeNames[i].index] = DimensionAndStartSize{ DimAndNodeNames[i].index ,start,size };
            }
        }
    }
}

pugi::xml_node CCziMetadataDocumentInfo::GetNode(const wchar_t* path) const
{
    return CCziMetadataDocumentInfo::GetNodeRelativeFromNode(this->metadata->GetXmlDoc().child(L"ImageDocument"), path);
}

/*static*/pugi::xml_node CCziMetadataDocumentInfo::GetNodeRelativeFromNode(pugi::xml_node node, const wchar_t* path)
{
    Utilities::Split(wstring(path), L'/',
        [&](const std::wstring str)->bool
        {
            node = node.child(str.c_str());
            return !node.empty();
        });

    return node;
}