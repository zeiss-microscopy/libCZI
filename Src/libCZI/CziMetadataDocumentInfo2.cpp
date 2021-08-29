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
#include "CziMetadataDocumentInfo2.h"

using namespace std;
using namespace libCZI;


template<typename t>
void SetIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, void(t::* pfn)(const wstring&))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto str = n.text().as_string();
		(p->*pfn)(str);
	}
}

template<typename t>
void SetIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, void(t::* pfn)(double))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto dbl = n.text().as_double();
		(p->*pfn)(dbl);
	}
}

template<typename t>
void SetIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, void(t::* pfn)(bool))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto dbl = n.text().as_bool();
		(p->*pfn)(dbl);
	}
}

template<typename t>
void SetIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, void(t::* pfn)(int))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto v = n.text().as_int();
		(p->*pfn)(v);
	}
}

template<typename t>
void SetIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, void(t::* pfn)(std::uint32_t))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto v = n.text().as_uint();
		(p->*pfn)(v);
	}
}

template<typename t, typename t2>
void SetEnumIfPresentAndValid(pugi::xml_node node, const wchar_t* nodeName, t* p, bool(*pfnStringToEnum)(const wchar_t*, t2*), void(t::* pfn)(t2))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto str = n.text().as_string();
		t2 e;
		if (pfnStringToEnum(str, &e))
		{
			(p->*pfn)(e);
		}
	}
}

template<typename t>
void SetIfAttributePresentAndValid(pugi::xml_node node, const wchar_t* nodeName, const wchar_t* attributeName, t* p, void(t::* pfn)(const wstring&))
{
	auto n = node.child(nodeName);
	if (!!n)
	{
		auto attr = n.attribute(attributeName);
		if (!!attr)
		{
			(p->*pfn)(attr.as_string());
		}
	}
}

static pugi::xml_node GetNodeRelativeFromNode(pugi::xml_node node, const wchar_t* path)
{
	Utilities::Split(wstring(path), L'/',
		[&](const std::wstring str)->bool
	{
		node = node.child(str.c_str());
		return !node.empty();
	});

	return node;
}

static pugi::xml_node GetNode(const CCziMetadata* md, const wchar_t* path)
{
	return GetNodeRelativeFromNode(md->GetXmlDoc().child(L"ImageDocument"), path);
}

// ************************************************************************************

void CAttributeId::ParseInformation(pugi::xml_node node)
{
	auto attr = node.attribute(L"Id");
	if (!!attr)
	{
		this->attributeId.Set(attr.as_string());
	}
}

void CAttributeName::ParseInformation(pugi::xml_node node)
{
	auto attr = node.attribute(L"Name");
	if (!!attr)
	{
		this->attributeName.Set(attr.as_string());
	}
}

// ************************************************************************************

CDimensionChannelInfo::CDimensionChannelInfo(pugi::xml_node node)
	: attributeId(node), attributeName(node)
{
	this->ParseInformation(node);
}

bool CDimensionChannelInfo::TryGetChannelType(libCZI::DimensionChannelChannelType* type) /*override*/
{
	return this->channeltype.TryGet(type);
}

bool CDimensionChannelInfo::TryGetChannelUnit(std::wstring* unit) /*override*/
{
	return this->channelunit.TryGet(unit);
}

bool CDimensionChannelInfo::TryGetPixelType(libCZI::PixelType* pixelType) /*override*/
{
	return this->pixeltype.TryGet(pixelType);
}

bool CDimensionChannelInfo::TryGetComponentBitCount(int* bitCount) /*override*/
{
	return this->bitcount.TryGet(bitCount);
}

bool CDimensionChannelInfo::TryGetAcquisitionMode(libCZI::DimensionChannelAcquisitionMode* acquisitionMode) /*override*/
{
	return this->acquisitionMode.TryGet(acquisitionMode);
}

bool CDimensionChannelInfo::TryGetIlluminationType(libCZI::DimensionChannelIlluminationType* illuminationType) /*override*/
{
	return this->illuminationType.TryGet(illuminationType);
}

bool CDimensionChannelInfo::TryGetContrastMethod(libCZI::DimensionChannelContrastMethod* contrastMethod) /*override*/
{
	return this->contrastMethod.TryGet(contrastMethod);
}

bool CDimensionChannelInfo::TryGetIlluminationWavelength(libCZI::SpectrumCharacteristics* illuminationwavelength) /*override*/
{
	return this->illuminationWavelength.TryGet(illuminationwavelength);
}

bool CDimensionChannelInfo::TryGetDetectionWavelength(libCZI::SpectrumCharacteristics* detectionwavelength) /*override*/
{
	return this->detectionWavelength.TryGet(detectionwavelength);
}

bool CDimensionChannelInfo::TryGetExcitationWavelength(double* excitationwavelength)  /*override*/
{
	return this->excitationWavelength.TryGet(excitationwavelength);
}

bool CDimensionChannelInfo::TryGetEmissionWavelength(double* emissionwavelength)  /*override*/
{
	return this->emissionWavelength.TryGet(emissionwavelength);
}

bool CDimensionChannelInfo::TryGetEffectiveNA(double* na)  /*override*/
{
	return this->effectiveNA.TryGet(na);
}

bool CDimensionChannelInfo::TryGetDyeId(std::wstring* dyeid) /*override*/
{
	return this->dyeId.TryGet(dyeid);
}

bool CDimensionChannelInfo::TryGetDyeDatabaseId(std::wstring* dyedatabaseid) /*override*/
{
	return this->dyeDatabaseId.TryGet(dyedatabaseid);
}

bool CDimensionChannelInfo::TryGetPinholeSize(double* pinholesize) /*override*/
{
	return this->pinholeSize.TryGet(pinholesize);
}

bool CDimensionChannelInfo::TryGetPinholeSizeAiry(double* pinholesizeairy) /*override*/
{
	return this->pinholeSizeAiry.TryGet(pinholesizeairy);
}

bool CDimensionChannelInfo::TryGetPinholeGeometry(libCZI::DimensionChannelPinholeGeometry* pinholegeometry) /*override*/
{
	return this->pinholeGeometry.TryGet(pinholegeometry);
}

bool CDimensionChannelInfo::TryGetFluor(std::wstring* fluor) /*override*/
{
	return this->fluor.TryGet(fluor);
}

bool CDimensionChannelInfo::TryGetNDFilter(double* ndfilter) /*override*/
{
	return this->ndFilter.TryGet(ndfilter);
}

bool CDimensionChannelInfo::TryGetPocketCellSetting(int* pocketcellsetting) /*override*/
{
	return this->pocketCellSetting.TryGet(pocketcellsetting);
}

bool CDimensionChannelInfo::TryGetColor(libCZI::Rgb8Color* color) /*override*/
{
	return this->color.TryGet(color);
}

bool CDimensionChannelInfo::TryGetExposureTime(libCZI::RangeOrSingleValue<std::uint64_t>* exposuretime) /*override*/
{
	return this->exposureTime.TryGet(exposuretime);
}

bool CDimensionChannelInfo::TryGetDepthOfFocus(double* depthoffocus) /*override*/
{
	return this->depthOfFocus.TryGet(depthoffocus);
}

bool CDimensionChannelInfo::TryGetSectionThickness(double* sectionthickness) /*override*/
{
	return this->sectionThickness.TryGet(sectionthickness);
}

std::shared_ptr<libCZI::IDimensionChannelDetectorSettings> CDimensionChannelInfo::GetDetectorSettings() /*override*/
{
	return this->detectorSettings;
}

std::shared_ptr<libCZI::IDimensionChannelLightSourcesSettings> CDimensionChannelInfo::GetLightSourcesSettings() /*override*/
{
	return this->lightSourcesSettings;
}

std::shared_ptr<libCZI::IDimensionChannelLightPath> CDimensionChannelInfo::GetLightPath() /*override*/
{
	return this->lighPath;
}

std::shared_ptr<IDimensionChannelLaserScanInfo> CDimensionChannelInfo::GetLaserScanInfo() /*override*/
{
	return this->laserScanInfo;
}

std::shared_ptr<IDimensionChannelSPIMIlluminationSettings> CDimensionChannelInfo::GetSPIMIlluminationSettings() /*override*/
{
	return this->spimIlluminationSettings;
}

std::shared_ptr<libCZI::IDimensionChannelSPIMDetectionSettings> CDimensionChannelInfo::GetSPIMDetectionSettings() /*override*/
{
	return this->spimDetectionSettings;
}

std::shared_ptr<libCZI::IDimensionChannelSIMSettings> CDimensionChannelInfo::GetSIMSettings() /*override*/
{
	return this->simSettings;
}

std::shared_ptr<libCZI::IDimensionChannelPolarizingSettings> CDimensionChannelInfo::GetPolarizingSettings() /*override*/
{
	return this->polarizingSettings;
}

std::shared_ptr<libCZI::IDimensionChannelAiryscanSettings> CDimensionChannelInfo::GetAiryscanSettings() /*override*/
{
	return this->airyscanSettings;
}

bool CDimensionChannelInfo::TryGetReflector(std::wstring* reflector) /*override*/
{
	return this->reflector.TryGet(reflector);
}

bool CDimensionChannelInfo::TryGetCondenserContrast(std::wstring* condensorContrast) /*override*/
{
	return this->condensorContrast.TryGet(condensorContrast);
}

bool CDimensionChannelInfo::TryGetNACondenser(double* naCondensor) /*override*/
{
	return this->naCondensor.TryGet(naCondensor);
}

std::shared_ptr<libCZI::IDimensionChannelRatio> CDimensionChannelInfo::GetRatio() /*override*/
{
	return this->ratio;
}
// ----------------------------------------------------------------------------

void CDimensionChannelInfo::SetChannelType(libCZI::DimensionChannelChannelType type)
{
	this->channeltype.Set(type);
}
void CDimensionChannelInfo::SetChannelUnit(const std::wstring& unit)
{
	this->channelunit.Set(unit);
}
void CDimensionChannelInfo::SetPixelType(libCZI::PixelType pixelType)
{
	this->pixeltype.Set(pixelType);
}
void CDimensionChannelInfo::SetComponentBitCount(int bitCount)
{
	this->bitcount.Set(bitCount);
}
void CDimensionChannelInfo::SetAcquisitionMode(libCZI::DimensionChannelAcquisitionMode acquisitionMode)
{
	this->acquisitionMode.Set(acquisitionMode);
}
void CDimensionChannelInfo::SetIlluminationType(libCZI::DimensionChannelIlluminationType illuminationType)
{
	this->illuminationType.Set(illuminationType);
}
void CDimensionChannelInfo::SetContrastMethod(libCZI::DimensionChannelContrastMethod contrastMethod)
{
	this->contrastMethod.Set(contrastMethod);
}
void CDimensionChannelInfo::SetIlluminationWavelength(const libCZI::SpectrumCharacteristics& illuminationwavelength)
{
	this->illuminationWavelength.Set(illuminationwavelength);
}
void CDimensionChannelInfo::SetDetectionWavelength(const libCZI::SpectrumCharacteristics& detectionwavelength)
{
	this->detectionWavelength.Set(detectionwavelength);
}
void CDimensionChannelInfo::SetExcitationWavelength(double excitationwavelength)
{
	this->excitationWavelength.Set(excitationwavelength);
}
void CDimensionChannelInfo::SetEmissionWavelength(double emissionwavelength)
{
	this->emissionWavelength.Set(emissionwavelength);
}
void CDimensionChannelInfo::SetEffectiveNA(double effectiveNA)
{
	this->effectiveNA.Set(effectiveNA);
}
void CDimensionChannelInfo::SetDyeId(const std::wstring& dyeid)
{
	this->dyeId.Set(dyeid);
}
void CDimensionChannelInfo::SetDyeDatabaseId(const std::wstring& dyedatabaseid)
{
	this->dyeDatabaseId.Set(dyedatabaseid);
}
void CDimensionChannelInfo::SetPinholeSize(double pinholeSize)
{
	this->pinholeSize.Set(pinholeSize);
}
void CDimensionChannelInfo::SetPinholeSizeAiry(double pinholeSizeAiry)
{
	this->pinholeSizeAiry.Set(pinholeSizeAiry);
}
void CDimensionChannelInfo::SetPinholeGeometry(DimensionChannelPinholeGeometry pinholeGeometry)
{
	this->pinholeGeometry.Set(pinholeGeometry);
}
void CDimensionChannelInfo::SetFluor(const std::wstring& fluor)
{
	this->fluor.Set(fluor);
}
void CDimensionChannelInfo::SetNDFilter(double ndfilter)
{
	this->ndFilter.Set(ndfilter);
}
void CDimensionChannelInfo::SetPockelCellSetting(int pocketcellsetting)
{
	this->pocketCellSetting.Set(pocketcellsetting);
}
void CDimensionChannelInfo::SetColor(const libCZI::Rgb8Color& color)
{
	this->color.Set(color);
}
void CDimensionChannelInfo::SetExposureTime(const libCZI::RangeOrSingleValue<std::uint64_t>& exposuretime)
{
	this->exposureTime.Set(exposuretime);
}
void CDimensionChannelInfo::SetDepthOfFocus(double depthoffocus)
{
	this->depthOfFocus.Set(depthoffocus);
}
void CDimensionChannelInfo::SetSectionThickness(double sectionthickness)
{
	this->sectionThickness.Set(sectionthickness);
}
void CDimensionChannelInfo::SetReflector(const std::wstring& reflector)
{
	this->reflector.Set(reflector);
}
void CDimensionChannelInfo::SetCondenserContrast(const std::wstring& condensorContrast)
{
	this->condensorContrast.Set(condensorContrast);
}
void CDimensionChannelInfo::SetNACondenser(double naCondensor)
{
	this->naCondensor.Set(naCondensor);
}
// ----------------------------------------------------------------------------

void CDimensionChannelInfo::ParseInformation(pugi::xml_node node)
{
	auto n = node.child(L"ChannelType");
	if (!!n)
	{
		auto str = n.text().as_string();
		DimensionChannelChannelType type;
		if (CXmlEnums::StringToDimensionChannelChannelType(str, &type))
		{
			this->SetChannelType(type);
		}
	}

	n = node.child(L"ChannelUnit");
	if (!!n)
	{
		this->SetChannelUnit(n.text().as_string());
	}

	n = node.child(L"PixelType");
	if (!!n)
	{
		auto str = n.text().as_string();
		PixelType pixeltype;
		if (CXmlEnums::StringToPixelType(str, &pixeltype))
		{
			this->SetPixelType(pixeltype);
		}
	}

	n = node.child(L"ComponentBitCount");
	if (!!n)
	{
		this->SetComponentBitCount(n.text().as_int());
	}

	n = node.child(L"AcquisitionMode");
	if (!!n)
	{
		auto str = n.text().as_string();
		DimensionChannelAcquisitionMode mode;
		if (CXmlEnums::StringToDimensionChannelAcquisitionMode(str, &mode))
		{
			this->SetAcquisitionMode(mode);
		}
	}

	n = node.child(L"IlluminationType");
	if (!!n)
	{
		auto str = n.text().as_string();
		DimensionChannelIlluminationType illuminationtype;
		if (CXmlEnums::StringToDimensionChannelIlluminationType(str, &illuminationtype))
		{
			this->SetIlluminationType(illuminationtype);
		}
	}

	n = node.child(L"ContrastMethod");
	if (!!n)
	{
		auto str = n.text().as_string();
		DimensionChannelContrastMethod contrastmethod;
		if (CXmlEnums::StringToDimensionChannelContrastMethod(str, &contrastmethod))
		{
			this->SetContrastMethod(contrastmethod);
		}
	}

	n = node.child(L"IlluminationWavelength");
	if (!!n)
	{
		SpectrumCharacteristics characteristics;
		if (TryParseSpectrumCharacteristic(n, &characteristics))
		{
			this->SetIlluminationWavelength(characteristics);
		}
	}

	n = node.child(L"DetectionWavelength");
	if (!!n)
	{
		SpectrumCharacteristics characteristics;
		if (TryParseSpectrumCharacteristic(n, &characteristics))
		{
			this->SetDetectionWavelength(characteristics);
		}
	}

	n = node.child(L"ExcitationWavelength");
	if (!!n)
	{
		this->SetExcitationWavelength(n.text().as_double());
	}

	n = node.child(L"EmissionWavelength");
	if (!!n)
	{
		this->SetEmissionWavelength(n.text().as_double());
	}

	n = node.child(L"EffectiveNA");
	if (!!n)
	{
		this->SetEffectiveNA(n.text().as_double());
	}

	n = node.child(L"DyeId");
	if (!!n)
	{
		this->SetDyeId(n.text().as_string());
	}

	n = node.child(L"DyeDatabaseId");
	if (!!n)
	{
		this->SetDyeDatabaseId(n.text().as_string());
	}

	n = node.child(L"PinholeSize");
	if (!!n)
	{
		this->SetPinholeSize(n.text().as_double());
	}

	n = node.child(L"PinholeSizeAiry");
	if (!!n)
	{
		this->SetPinholeSizeAiry(n.text().as_double());
	}

	n = node.child(L"PinholeGeometry");
	if (!!n)
	{
		DimensionChannelPinholeGeometry pinholeGeometry;
		if (CXmlEnums::StringToDimensionChannelPinholeGeometry(n.text().as_string(), &pinholeGeometry))
		{
			this->SetPinholeGeometry(pinholeGeometry);
		}
	}

	n = node.child(L"Fluor");
	if (!!n)
	{
		this->SetFluor(n.text().as_string());
	}

	n = node.child(L"NDFilter");
	if (!!n)
	{
		this->SetNDFilter(n.text().as_double());
	}

	n = node.child(L"PockelCellSetting");
	if (!!n)
	{
		this->SetPockelCellSetting(n.text().as_int());
	}

	n = node.child(L"Color");
	if (!!n)
	{
		Rgb8Color color;
		if (Utilities::TryGetRgb8ColorFromString(n.text().as_string(), color))
		{
			this->SetColor(color);
		}
	}

	n = node.child(L"ExposureTime");
	if (!!n)
	{
		wstring str{ n.text().as_string() };
		Utilities::RemoveSpaces(str);
		if (!str.empty())
		{
			RangeOrSingleValue<std::uint64_t> rangeOrSingleValue;
			if (TryParseRangeOrSingleValue(str, &rangeOrSingleValue))
			{
				this->SetExposureTime(rangeOrSingleValue);
			}
		}
	}

	n = node.child(L"DepthOfFocus");
	if (!!n)
	{
		this->SetDepthOfFocus(n.text().as_double());
	}

	n = node.child(L"SectionThickness");
	if (!!n)
	{
		this->SetSectionThickness(n.text().as_double());
	}

	n = node.child(L"DetectorSettings");
	if (!!n)
	{
		this->detectorSettings = make_shared<CDimensionChannelDetectorSettings>(n);
	}

	n = node.child(L"LightSourcesSettings");
	if (!!n)
	{
		this->lightSourcesSettings = make_shared<CDimensionChannelLightSourcesSettings>(n);
	}

	n = node.child(L"LightPath");
	if (!!n)
	{
		this->lighPath = make_shared<CDimensionChannelLightPath>(n);
	}

	n = node.child(L"LaserScanInfo");
	if (!!n)
	{
		this->laserScanInfo = make_shared<CDimensionChannelLaserScanInfo>(n);
	}

	n = node.child(L"SPIMIlluminationSettings");
	if (!!n)
	{
		this->spimIlluminationSettings = make_shared<CDimensionChannelSPIMIlluminationSettings>(n);
	}

	n = node.child(L"SPIMDetectionSettings");
	if (!!n)
	{
		this->spimDetectionSettings = make_shared<CDimensionChannelSPIMDetectionSettings>(n);
	}

	n = node.child(L"SIMSettings");
	if (!!n)
	{
		this->simSettings = make_shared<CDimensionChannelSIMSettings>(n);
	}

	n = node.child(L"PolarizingSettings");
	if (!!n)
	{
		this->polarizingSettings = make_shared<CDimensionChannelPolarizingSettings>(n);
	}

	n = node.child(L"AiryscanSettings");
	if (!!n)
	{
		this->airyscanSettings = make_shared< CDimensionChannelAiryscanSettings>(n);
	}

	SetIfPresentAndValid(node, L"Reflector", this, &CDimensionChannelInfo::SetReflector);
	SetIfPresentAndValid(node, L"CondenserContrast", this, &CDimensionChannelInfo::SetCondenserContrast);
	SetIfPresentAndValid(node, L"NACondenser", this, &CDimensionChannelInfo::SetNACondenser);

	n = node.child(L"Ratio");
	if (!!n)
	{
		this->ratio = make_shared<CDimensionChannelRatio>(n);
	}
}

/*static*/bool CDimensionChannelInfo::TryParseSpectrumCharacteristic(pugi::xml_node node, libCZI::SpectrumCharacteristics* spectrumCharacteristic)
{
	auto singlePeakNode = node.child(L"SinglePeak");
	if (!!singlePeakNode)
	{
		if (spectrumCharacteristic != nullptr)
		{
			spectrumCharacteristic->type = SpectrumCharacteristics::InformationType::SinglePeak;
			spectrumCharacteristic->singlePeak = singlePeakNode.text().as_double();
		}

		return true;
	}

	auto rangesNode = node.child(L"Ranges");
	if (!!rangesNode)
	{
		vector<wstring> parts;
		Utilities::Split(
			rangesNode.text().as_string(),
			',',
			[&](const std::wstring p)->bool
		{
			if (!p.empty())
			{
				parts.emplace_back(p);
			}

			return true;
		});

		if (parts.size() == 0)
		{
			return false;
		}

		for (auto&& s : parts) { Utilities::RemoveSpaces(s); }

		if (spectrumCharacteristic != nullptr)
		{
			spectrumCharacteristic->ranges.clear();
		}

		for (const auto& s : parts)
		{
			RangeOrSingleValue<double> rangeOrSingleValue;
			if (TryParseRangeOrSingleValue(s, &rangeOrSingleValue) == false)
			{
				return false;
			}

			if (spectrumCharacteristic != nullptr)
			{
				spectrumCharacteristic->ranges.emplace_back(rangeOrSingleValue);
			}
		}

		if (spectrumCharacteristic != nullptr)
		{
			spectrumCharacteristic->type = SpectrumCharacteristics::InformationType::Ranges;
		}

		return true;
	}

	return false;
}

/*static*/bool CDimensionChannelInfo::TryParseRangeOrSingleValue(const std::wstring& str, libCZI::RangeOrSingleValue<double>* rangeOrSingleValue)
{
	// preconditions:
	// - str is not empty
	// - str does not contain an whitespaces
	try
	{
		if (str[0] == L'-')
		{
			size_t s;
			double end = std::stod(str.c_str() + 1, &s);
			if (1 + s == str.length())
			{
				rangeOrSingleValue->singleValue = false;
				rangeOrSingleValue->startOrSingleValue = (std::numeric_limits<double>::min)();;
				rangeOrSingleValue->end = end;
				return true;
			}
		}
		else
		{
			size_t s;
			double start = std::stod(str.c_str(), &s);
			if (s < str.length())
			{
				if (str[s] != L'-')
				{
					return false;
				}

				if (str[s + 1] == L'\0')
				{
					rangeOrSingleValue->singleValue = false;
					rangeOrSingleValue->startOrSingleValue = start;
					rangeOrSingleValue->end = (std::numeric_limits<double>::max)();
					return true;
				}
				else
				{
					size_t s2;
					double end = std::stod(str.c_str() + s + 1, &s2);
					if (1 + s + s2 == str.length())
					{
						rangeOrSingleValue->singleValue = false;
						rangeOrSingleValue->startOrSingleValue = start;
						rangeOrSingleValue->end = end;
						return true;
					}
				}
			}
			else if (s == str.length())
			{
				if (rangeOrSingleValue != nullptr)
				{
					rangeOrSingleValue->singleValue = true;
					rangeOrSingleValue->startOrSingleValue = start;
					rangeOrSingleValue->end = std::numeric_limits<double>::quiet_NaN();
					return true;
				}
			}
		}

		return false;
	}
	catch (std::invalid_argument&)
	{
		return false;
	}
	catch (std::out_of_range&)
	{
		return false;
	}
}

/*static*/bool CDimensionChannelInfo::TryParseRangeOrSingleValue(const std::wstring& str, libCZI::RangeOrSingleValue<std::uint64_t>* rangeOrSingleValue)
{
	// preconditions:
	// - str is not empty
	// - str does not contain an whitespaces
	try
	{
		if (str[0] == L'-')
		{
			size_t s;
			uint64_t end = std::stoull(str.c_str() + 1, &s, 10);
			if (1 + s == str.length())
			{
				rangeOrSingleValue->singleValue = false;
				rangeOrSingleValue->startOrSingleValue = (std::numeric_limits<uint64_t>::min)();;
				rangeOrSingleValue->end = end;
				return true;
			}
		}
		else
		{
			size_t s;
			uint64_t start = std::stoull(str.c_str(), &s, 10);
			if (s < str.length())
			{
				if (str[s] != L'-')
				{
					return false;
				}

				if (str[s + 1] == L'\0')
				{
					rangeOrSingleValue->singleValue = false;
					rangeOrSingleValue->startOrSingleValue = start;
					rangeOrSingleValue->end = (std::numeric_limits<uint64_t>::max)();
					return true;
				}
				else
				{
					size_t s2;
					uint64_t end = std::stoull(str.c_str() + s + 1, &s2, 10);
					if (1 + s + s2 == str.length())
					{
						rangeOrSingleValue->singleValue = false;
						rangeOrSingleValue->startOrSingleValue = start;
						rangeOrSingleValue->end = end;
						return true;
					}
				}
			}
			else if (s == str.length())
			{
				if (rangeOrSingleValue != nullptr)
				{
					rangeOrSingleValue->singleValue = true;
					rangeOrSingleValue->startOrSingleValue = start;
					rangeOrSingleValue->end = 0;
					return true;
				}
			}
		}

		return false;
	}
	catch (std::invalid_argument&)
	{
		return false;
	}
	catch (std::out_of_range&)
	{
		return false;
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CDimensionsChannelsInfo::CDimensionsChannelsInfo(pugi::xml_node node)
{
	this->ParseInformation(node);
}

int CDimensionsChannelsInfo::GetChannelCount() const /*override*/
{
	return (int)this->channels.size();
}

std::shared_ptr<IDimensionChannelInfo> CDimensionsChannelsInfo::GetChannel(int i) const /*override*/
{
	return this->channels[i];
}

/*static*/std::shared_ptr<CDimensionsChannelsInfo> CDimensionsChannelsInfo::TryParse(const CCziMetadata* md)
{
	auto dimCnode = GetNode(md, L"Metadata/Information/Image/Dimensions/Channels");
	if (!dimCnode)
	{
		return shared_ptr<CDimensionsChannelsInfo>();
	}

	return make_shared<CDimensionsChannelsInfo>(dimCnode);
}

void CDimensionsChannelsInfo::ParseInformation(pugi::xml_node node)
{
	static const wchar_t* ChannelNodeName = L"Channel";
	for (pugi::xml_node ch = node.child(ChannelNodeName); ch; ch = ch.next_sibling(ChannelNodeName))
	{
		this->channels.emplace_back(make_shared<CDimensionChannelInfo>(ch));
	}
}

// ----------------------------------------------------------------------------

CDimensionChannelDetectorSettings::CDimensionChannelDetectorSettings(pugi::xml_node node)
{
	this->ParseInformation(node);
}

bool CDimensionChannelDetectorSettings::TryGetDetectorRef(std::wstring* detectorRef) /*override*/
{
	return this->detectorRef.TryGet(detectorRef);
}
bool CDimensionChannelDetectorSettings::TryGetBinning(ChannelDetectorSettingsBinning* binning) /*override*/
{
	return this->binning.TryGet(binning);
}
bool CDimensionChannelDetectorSettings::TryGetShutter(ChannelDetectorSettingsShutter* shutter) /*override*/
{
	return this->shutter.TryGet(shutter);
}
bool CDimensionChannelDetectorSettings::TryGetPhotonConversionFactor(double* conversionFactor) /*override*/
{
	return this->photonConversionFactor.TryGet(conversionFactor);
}
bool CDimensionChannelDetectorSettings::TryGetGain(double* gain) /*override*/
{
	return this->gain.TryGet(gain);
}
bool CDimensionChannelDetectorSettings::TryGetDigitalGain(double* digitalgain) /*override*/
{
	return this->digitalGain.TryGet(digitalgain);
}
bool CDimensionChannelDetectorSettings::TryGetOffset(double* offset) /*override*/
{
	return this->offset.TryGet(offset);
}
bool CDimensionChannelDetectorSettings::TryGetEMGain(double* emgain) /*override*/
{
	return this->emGain.TryGet(emgain);
}
bool CDimensionChannelDetectorSettings::TryGetVoltage(double* voltage) /*override*/
{
	return this->voltage.TryGet(voltage);
}
bool CDimensionChannelDetectorSettings::TryGetReadOutRate(double* readoutrate) /*override*/
{
	return this->readOutRate.TryGet(readoutrate);
}
bool CDimensionChannelDetectorSettings::TryGetUseBrightnessContrastCorrection(bool* usebrightnesscontrastcorrection) /*override*/
{
	return this->useBrightnessContrastCorrection.TryGet(usebrightnesscontrastcorrection);
}
bool CDimensionChannelDetectorSettings::TryGetPixelMaximum(double* pixelmaximum) /*override*/
{
	return this->pixelMaximum.TryGet(pixelmaximum);
}
bool CDimensionChannelDetectorSettings::TryGetPixelAccuracy(double* pixelaccuracy) /*override*/
{
	return this->pixelAccuracy.TryGet(pixelaccuracy);
}
bool CDimensionChannelDetectorSettings::TryGetPixelScaleFactor(double* pixelscalefactor) /*override*/
{
	return this->pixelScaleFactor.TryGet(pixelscalefactor);
}
bool CDimensionChannelDetectorSettings::TryGetCameraProcessingMode(ChannelDetectorCameraProcessingMode* mode) /*override*/
{
	return this->cameraProcessingMode.TryGet(mode);
}

void CDimensionChannelDetectorSettings::ParseInformation(pugi::xml_node node)
{
	SetIfAttributePresentAndValid(node, L"Detector", L"Id", this, &CDimensionChannelDetectorSettings::SetDetectorRef);
	SetEnumIfPresentAndValid(node, L"Binning", this, CXmlEnums::StringToBinning, &CDimensionChannelDetectorSettings::SetBinning);
	SetEnumIfPresentAndValid(node, L"Shutter", this, CXmlEnums::StringToShutter, &CDimensionChannelDetectorSettings::SetShutter);
	SetIfPresentAndValid(node, L"PhotonConversionFactor", this, &CDimensionChannelDetectorSettings::SetPhotonConversionFactor);
	SetIfPresentAndValid(node, L"Gain", this, &CDimensionChannelDetectorSettings::SetGain);
	SetIfPresentAndValid(node, L"DigitalGain", this, &CDimensionChannelDetectorSettings::SetDigitalGain);
	SetIfPresentAndValid(node, L"Offset", this, &CDimensionChannelDetectorSettings::SetOffset);
	SetIfPresentAndValid(node, L"EMGain", this, &CDimensionChannelDetectorSettings::SetEmGain);
	SetIfPresentAndValid(node, L"Voltage", this, &CDimensionChannelDetectorSettings::SetVoltage);
	SetIfPresentAndValid(node, L"ReadOutRate", this, &CDimensionChannelDetectorSettings::SetReadOutRate);
	SetIfPresentAndValid(node, L"UseBrightnessContrastCorrection", this, &CDimensionChannelDetectorSettings::SetUseBrightnessContrastCorrection);
	SetIfPresentAndValid(node, L"PixelMaximum", this, &CDimensionChannelDetectorSettings::SetPixelMaximum);
	SetIfPresentAndValid(node, L"PixelAccuracy", this, &CDimensionChannelDetectorSettings::SetPixelAccuracy);
	SetIfPresentAndValid(node, L"PixelScaleFactor", this, &CDimensionChannelDetectorSettings::SetPixelScaleFactor);
	SetEnumIfPresentAndValid(node, L"CameraProcessingMode", this, CXmlEnums::StringToCameraProcessingMode, &CDimensionChannelDetectorSettings::SetCameraProcessingMode);
}

// ----------------------------------------------------------------------------

CDimensionChannelLightSourcesSetting::CDimensionChannelLightSourcesSetting(pugi::xml_node node)
{
	this->ParseInformation(node);
}

bool CDimensionChannelLightSourcesSetting::TryGetLightSourceRef(std::wstring* lightsourceref) /*override*/
{
	return this->lightSourceRef.TryGet(lightsourceref);
}
bool CDimensionChannelLightSourcesSetting::TryGetWavelength(double* wavelength) /*override*/
{
	return this->waveLength.TryGet(wavelength);
}
bool CDimensionChannelLightSourcesSetting::TryGetAttenuation(double* attenuation) /*override*/
{
	return this->attenuation.TryGet(attenuation);
}
bool CDimensionChannelLightSourcesSetting::TryGetTransmission(double* transmission) /*override*/
{
	return this->transmission.TryGet(transmission);
}
bool CDimensionChannelLightSourcesSetting::TryGetIntensity(std::wstring* intensity) /*override*/
{
	return this->intensity.TryGet(intensity);
}
bool CDimensionChannelLightSourcesSetting::TryGetFlashDuration(double* flashduration) /*override*/
{
	return this->flashDuration.TryGet(flashduration);
}
bool CDimensionChannelLightSourcesSetting::TryGetTIRFAngle(double* tirfangle) /*override*/
{
	return this->tirfAngle.TryGet(tirfangle);
}
bool CDimensionChannelLightSourcesSetting::TryGetTIRFDepthOfPenetration(double* tirfdepthofpenetration) /*override*/
{
	return this->tirfDepthOfPenetration.TryGet(tirfdepthofpenetration);
}
bool CDimensionChannelLightSourcesSetting::TryGetIsTIRFCalibrated(bool* isTIRFcalibrated) /*override*/
{
	return this->isTIRFCalibrated.TryGet(isTIRFcalibrated);
}
bool CDimensionChannelLightSourcesSetting::TryGetIsNlo(bool* isnlo) /*override*/
{
	return this->isNLO.TryGet(isnlo);
}
bool CDimensionChannelLightSourcesSetting::TryGetPolarization(Polarization* polarization) /*override*/
{
	return this->polarization.TryGet(polarization);
}

void CDimensionChannelLightSourcesSetting::ParseInformation(pugi::xml_node node)
{
	SetIfAttributePresentAndValid(node, L"LightSource", L"Id", this, &CDimensionChannelLightSourcesSetting::SetLightSourceRef);
	SetIfPresentAndValid(node, L"Wavelength", this, &CDimensionChannelLightSourcesSetting::SetWavelength);
	SetIfPresentAndValid(node, L"Attenuation", this, &CDimensionChannelLightSourcesSetting::SetAttenuation);
	SetIfPresentAndValid(node, L"Transmission", this, &CDimensionChannelLightSourcesSetting::SetTransmission);
	SetIfPresentAndValid(node, L"Intensity", this, &CDimensionChannelLightSourcesSetting::SetIntensity);
	SetIfPresentAndValid(node, L"FlashDuration", this, &CDimensionChannelLightSourcesSetting::SetFlashDuration);
	SetIfPresentAndValid(node, L"TIRFAngle", this, &CDimensionChannelLightSourcesSetting::SetTIRFAngle);
	SetIfPresentAndValid(node, L"TIRFDepthOfPenetration", this, &CDimensionChannelLightSourcesSetting::SetTIRFDepthOfPenetration);
	SetIfPresentAndValid(node, L"IsTIRFCalibrated", this, &CDimensionChannelLightSourcesSetting::SetIsTIRFCalibrated);
	SetIfPresentAndValid(node, L"IsNlo", this, &CDimensionChannelLightSourcesSetting::SetIsNlo);

	auto n = node.child(L"Polarization");
	if (!!n)
	{
		Polarization polarization;
		CDimensionChannelLightSourcesSetting::Parse(n, polarization);
		this->SetPolarization(polarization);
	}
}

/*static*/void CDimensionChannelLightSourcesSetting::Parse(pugi::xml_node node, Polarization& polarization)
{
	for (int i = 0; i < 4; ++i)
	{
		polarization.stokesParameterValid[i] = false;
		polarization.stokesParameter[i] = numeric_limits<double>::quiet_NaN();
	}

	auto n = node.child(L"StokesParameter1");
	if (!!n)
	{
		polarization.stokesParameterValid[0] = true;
		polarization.stokesParameter[0] = n.text().as_double();
	}

	n = node.child(L"StokesParameter2");
	if (!!n)
	{
		polarization.stokesParameterValid[1] = true;
		polarization.stokesParameter[1] = n.text().as_double();
	}

	n = node.child(L"StokesParameter3");
	if (!!n)
	{
		polarization.stokesParameterValid[2] = true;
		polarization.stokesParameter[2] = n.text().as_double();
	}

	n = node.child(L"StokesParameter4");
	if (!!n)
	{
		polarization.stokesParameterValid[3] = true;
		polarization.stokesParameter[3] = n.text().as_double();
	}
}

// ----------------------------------------------------------------------------

CDimensionChannelLightSourcesSettings::CDimensionChannelLightSourcesSettings(pugi::xml_node node)
{
	this->ParseInformation(node);
}

int CDimensionChannelLightSourcesSettings::GetLightSourceSettingCount() const /*override*/
{
	return (int)this->lightSourceSettings.size();
}

std::shared_ptr<IDimensionChannelLightSourcesSetting> CDimensionChannelLightSourcesSettings::GetLightSourceSetting(int i) const /*override*/
{
	return this->lightSourceSettings[i];
}

void CDimensionChannelLightSourcesSettings::ParseInformation(pugi::xml_node node)
{
	static const wchar_t* LightSourceSettingsNode = L"LightSourceSettings";
	for (pugi::xml_node lightSourceSettings = node.child(LightSourceSettingsNode);
		lightSourceSettings;
		lightSourceSettings = lightSourceSettings.next_sibling(LightSourceSettingsNode))
	{
		this->lightSourceSettings.emplace_back(make_shared<CDimensionChannelLightSourcesSetting>(lightSourceSettings));
	}
}

// ----------------------------------------------------------------------------

CDimensionChannelLightPath::CDimensionChannelLightPath(pugi::xml_node node)
{
	this->ParseInformation(node);
}

bool CDimensionChannelLightPath::TryGetExcitationFilterRef(std::wstring* excitationFilterRef) /*override*/
{
	return this->excitationFilterRef.TryGet(excitationFilterRef);
}
bool CDimensionChannelLightPath::TryGetDichroicFilterRef(std::wstring* dichroicRef) /*override*/
{
	return this->dichroicRef.TryGet(dichroicRef);
}
bool CDimensionChannelLightPath::TryGetEmissionFilterRef(std::wstring* emissionFilterRef) /*override*/
{
	return this->emissionFilterRef.TryGet(emissionFilterRef);
}

void CDimensionChannelLightPath::ParseInformation(pugi::xml_node node)
{
	SetIfAttributePresentAndValid(node, L"ExcitationFilter", L"Id", this, &CDimensionChannelLightPath::SetExcitationFilterRef);
	SetIfAttributePresentAndValid(node, L"Dichroic", L"Id", this, &CDimensionChannelLightPath::SetDichroicFilterRef);
	SetIfAttributePresentAndValid(node, L"EmissionFilter", L"Id", this, &CDimensionChannelLightPath::SetEmissionFilterRef);
}
// ----------------------------------------------------------------------------

CDimensionChannelLaserScanInfo::CDimensionChannelLaserScanInfo(pugi::xml_node node)
{
	this->ParseInformation(node);
}

bool CDimensionChannelLaserScanInfo::TryGetPixelTime(double* pixelTime) /*override*/
{
	return this->pixelTime.TryGet(pixelTime);
}
bool CDimensionChannelLaserScanInfo::TryGetLineTime(double* lineTime) /*override*/
{
	return this->lineTime.TryGet(lineTime);
}
bool CDimensionChannelLaserScanInfo::TryGetFrameTime(double* frameTime) /*override*/
{
	return this->frameTime.TryGet(frameTime);
}
bool CDimensionChannelLaserScanInfo::TryGetAveraging(int* averaging) /*override*/
{
	return this->averaging.TryGet(averaging);
}
bool CDimensionChannelLaserScanInfo::TryGetScanningMode(libCZI::DimensionChannelLaserScanScanningMode* mode) /*override*/
{
	return this->laserScanningMode.TryGet(mode);
}
bool CDimensionChannelLaserScanInfo::TryGetSampleRotation(double* sampleRotation) /*override*/
{
	return this->sampleRotation.TryGet(sampleRotation);
}
bool CDimensionChannelLaserScanInfo::TryGetSampleOffsetX(double* sampleOffsetX) /*override*/
{
	return this->sampleOffsetX.TryGet(sampleOffsetX);
}
bool CDimensionChannelLaserScanInfo::TryGetSampleOffsetY(double* sampleOffsetY) /*override*/
{
	return this->sampleOffsetY.TryGet(sampleOffsetY);
}
bool CDimensionChannelLaserScanInfo::TryGetScanDirection(libCZI::DimensionChannelLaserScanScanDirection* scanDirection) /*override*/
{
	return this->scanDirection.TryGet(scanDirection);
}
bool CDimensionChannelLaserScanInfo::TryGetZoomX(double* zoomX) /*override*/
{
	return this->zoomX.TryGet(zoomX);
}
bool CDimensionChannelLaserScanInfo::TryGetZoomY(double* zoomY) /*override*/
{
	return this->zoomY.TryGet(zoomY);
}
bool CDimensionChannelLaserScanInfo::TryGetZoomZ(double* zoomZ) /*override*/
{
	return this->zoomZ.TryGet(zoomZ);
}
bool CDimensionChannelLaserScanInfo::TryGetCropOffset(double* cropOffset) /*override*/
{
	return this->cropOffset.TryGet(cropOffset);
}
bool CDimensionChannelLaserScanInfo::TryGetLaserAttenuatorMeas(double* laserAttenuatorMeas) /*override*/
{
	return this->laserAttenuatorMeas.TryGet(laserAttenuatorMeas);
}
bool CDimensionChannelLaserScanInfo::TryGetLaserBlanking(double* laserBlanking) /*override*/
{
	return this->laserBlanking.TryGet(laserBlanking);
}
bool CDimensionChannelLaserScanInfo::TryGetLaserAttenuatorBleach(double* laserAttenuatorBleach) /*override*/
{
	return this->laserAttenuatorBleack.TryGet(laserAttenuatorBleach);
}
bool CDimensionChannelLaserScanInfo::TryGetLineStep(std::uint32_t* lineStep) /*override*/
{
	return this->lineStep.TryGet(lineStep);
}
bool CDimensionChannelLaserScanInfo::TryGetScanSpeed(int* scanSpeed) /*override*/
{
	return this->scanSpeed.TryGet(scanSpeed);
}
bool CDimensionChannelLaserScanInfo::TryGetAveragingMethod(libCZI::DimensionChannelLaserScanAveragingMethod* averagingMethod) /*override*/
{
	return this->averagingMethod.TryGet(averagingMethod);
}
bool CDimensionChannelLaserScanInfo::TryGetAveragingMode(libCZI::DimensionChannelLaserScanAveragingMode* averagingMode) /*override*/
{
	return this->averagingMode.TryGet(averagingMode);
}
bool CDimensionChannelLaserScanInfo::TryGetLaserAttenuatorState(libCZI::DimensionChannelLaserScanLaserAttenuatorState* attenuatorState) /*override*/
{
	return this->attenuatorState.TryGet(attenuatorState);
}

void CDimensionChannelLaserScanInfo::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"PixelTime", this, &CDimensionChannelLaserScanInfo::SetPixelTime);
	SetIfPresentAndValid(node, L"LineTime", this, &CDimensionChannelLaserScanInfo::SetLineTime);
	SetIfPresentAndValid(node, L"FrameTime", this, &CDimensionChannelLaserScanInfo::SetFrameTime);
	SetIfPresentAndValid(node, L"Averaging", this, &CDimensionChannelLaserScanInfo::SetAveraging);
	SetEnumIfPresentAndValid(node, L"ScanningMode", this, CXmlEnums::StringToDimensionChannelLaserScanScanningMode, &CDimensionChannelLaserScanInfo::SetScanningMode);
	SetIfPresentAndValid(node, L"SampleRotation", this, &CDimensionChannelLaserScanInfo::SetSampleRotation);
	SetIfPresentAndValid(node, L"SampleOffsetX", this, &CDimensionChannelLaserScanInfo::SetSampleOffsetX);
	SetIfPresentAndValid(node, L"SampleOffsetY", this, &CDimensionChannelLaserScanInfo::SetSampleOffsetY);
	SetEnumIfPresentAndValid(node, L"ScanDirection", this, CXmlEnums::StringToDimensionChannelLaserScanScanDirection, &CDimensionChannelLaserScanInfo::SetScanDirection);
	SetIfPresentAndValid(node, L"ZoomX", this, &CDimensionChannelLaserScanInfo::SetZoomX);
	SetIfPresentAndValid(node, L"ZoomY", this, &CDimensionChannelLaserScanInfo::SetZoomY);
	SetIfPresentAndValid(node, L"ZoomZ", this, &CDimensionChannelLaserScanInfo::SetZoomZ);
	SetIfPresentAndValid(node, L"CropOffset", this, &CDimensionChannelLaserScanInfo::SetCropOffset);
	SetIfPresentAndValid(node, L"LaserAttenuatorMeas", this, &CDimensionChannelLaserScanInfo::SetLaserAttenuatorMeas);
	SetIfPresentAndValid(node, L"LaserBlanking", this, &CDimensionChannelLaserScanInfo::SetLaserBlanking);
	SetIfPresentAndValid(node, L"LaserAttenuatorBleach", this, &CDimensionChannelLaserScanInfo::SetLaserAttenuatorBleach);
	SetIfPresentAndValid(node, L"LineStep", this, &CDimensionChannelLaserScanInfo::SetLineStep);
	SetIfPresentAndValid(node, L"ScanSpeed", this, &CDimensionChannelLaserScanInfo::SetScanSpeed);
	SetEnumIfPresentAndValid(node, L"AveragingMethod", this, CXmlEnums::StringToDimensionChannelLaserScanAveragingMethod, &CDimensionChannelLaserScanInfo::SetAveragingMethod);
	SetEnumIfPresentAndValid(node, L"AveragingMode", this, CXmlEnums::StringToDimensionChannelLaserScanAveragingMode, &CDimensionChannelLaserScanInfo::SetAveragingMode);
	SetEnumIfPresentAndValid(node, L"LaserAttenuatorState", this, CXmlEnums::StringToDimensionChannelLaserScanLaserAttenuatorState, &CDimensionChannelLaserScanInfo::SetLaserAttenuatorState);
}

// ----------------------------------------------------------------------------

void CDimensionChannelSPIMIlluminationSettings::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"NALightSheet", this, &CDimensionChannelSPIMIlluminationSettings::SetNALightSheet);
	SetEnumIfPresentAndValid(node, L"BeamType", this, CXmlEnums::StringToDimensionChannelSPIMIlluminationSettingsBeamType, &CDimensionChannelSPIMIlluminationSettings::SetBeamType);
	SetIfAttributePresentAndValid(node, L"CylinderLensRef", L"Id", this, &CDimensionChannelSPIMIlluminationSettings::SetCylinderLensRef);
	auto n = node.child(L"IlluminationObjectiveSettings");
	if (!!n)
	{
		this->illuminationObjectiveSettings = make_shared<CInstrumentObjectiveSettings>(n);
	}

	n = node.child(L"ResonanceScannerSettings");
	if (!!n)
	{
		this->illuminationResonanceScannerSettings = make_shared<CSPIMIlluminationResonanceScannerSettings>(n);
	}

	n = node.child(L"GalvoScannerSettings");
	if (!!n)
	{
		this->illuminationGalvoScannerSettings = make_shared<CSPIMIlluminationGalvoScannerSettings>(n);
	}
}

// ----------------------------------------------------------------------------

void CInstrumentObjectiveSettings::ParseInformation(pugi::xml_node node)
{
	SetIfAttributePresentAndValid(node, L"ObjectiveRef", L"Id", this, &CInstrumentObjectiveSettings::SetObjectiveRef);
	SetEnumIfPresentAndValid(node, L"Medium", this, CXmlEnums::StringToIInstrumentObjectiveSettingsMedium, &CInstrumentObjectiveSettings::SetMedium);
	SetIfPresentAndValid(node, L"CorrectionCollar", this, &CInstrumentObjectiveSettings::SetCorrectionCollar);
	SetIfPresentAndValid(node, L"CorrectionCollar2", this, &CInstrumentObjectiveSettings::SetCorrectionCollar2);
	SetIfPresentAndValid(node, L"CorrectionControllerMode", this, &CInstrumentObjectiveSettings::SetCorrectionControllerMode);
	SetIfPresentAndValid(node, L"CorrectionControllerBottomThickness", this, &CInstrumentObjectiveSettings::SetCorrectionControllerBottomThickness);
	SetIfPresentAndValid(node, L"CorrectionControllerImagingDepth", this, &CInstrumentObjectiveSettings::SetCorrectionControllerImagingDepth);
	SetIfPresentAndValid(node, L"RefractiveIndex", this, &CInstrumentObjectiveSettings::SetRefractiveIndex);
}

// ----------------------------------------------------------------------------

void CSPIMIlluminationResonanceScannerSettings::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"Amplitude", this, &CSPIMIlluminationResonanceScannerSettings::SetAmplitude);
}

// ----------------------------------------------------------------------------

void CSPIMIlluminationGalvoScannerSettings::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"AmplitudeYScan", this, &CSPIMIlluminationGalvoScannerSettings::SetAmplitudeYScan);
	SetIfPresentAndValid(node, L"Frequency", this, &CSPIMIlluminationGalvoScannerSettings::SetFrequency);
	SetIfPresentAndValid(node, L"Offset1Left", this, &CSPIMIlluminationGalvoScannerSettings::SetOffset1Left);
	SetIfPresentAndValid(node, L"Offset1Right", this, &CSPIMIlluminationGalvoScannerSettings::SetOffset1Right);
	SetIfPresentAndValid(node, L"Offset2Left", this, &CSPIMIlluminationGalvoScannerSettings::SetOffset2Left);
	SetIfPresentAndValid(node, L"Offset2Right", this, &CSPIMIlluminationGalvoScannerSettings::SetOffset2Right);
}

// ----------------------------------------------------------------------------

void CDimensionChannelSPIMDetectionSettings::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"NADetection", this, &CDimensionChannelSPIMDetectionSettings::SetNADetection);
}

// ----------------------------------------------------------------------------

void CDimensionChannelSIMSettings::ParseInformation(pugi::xml_node node)
{
	SetIfAttributePresentAndValid(node, L"SIMDeviceRef", L"Id", this, &CDimensionChannelSIMSettings::SetSIMDeviceRef);
	SetIfPresentAndValid(node, L"GratingPeriod", this, &CDimensionChannelSIMSettings::SetGratingPeriod);
	SetIfPresentAndValid(node, L"GratingRotation", this, &CDimensionChannelSIMSettings::SetGratingRotation);
	SetIfPresentAndValid(node, L"GridDescription", this, &CDimensionChannelSIMSettings::SetGridDescription);
	SetIfPresentAndValid(node, L"Reflector", this, &CDimensionChannelSIMSettings::SetReflector);
	SetIfAttributePresentAndValid(node, L"FilterSetRef", L"Id", this, &CDimensionChannelSIMSettings::SetFilterSetRef);
	SetIfPresentAndValid(node, L"DiffractionEfficiency1", this, &CDimensionChannelSIMSettings::SetDiffractionEfficiency1);
	SetIfPresentAndValid(node, L"DiffractionEfficiency2", this, &CDimensionChannelSIMSettings::SetDiffractionEfficiency2);
	SetIfPresentAndValid(node, L"DiffractionEfficiency3", this, &CDimensionChannelSIMSettings::SetDiffractionEfficiency3);
}

// ----------------------------------------------------------------------------

void CDimensionChannelAiryscanSettings::ParseInformation(pugi::xml_node node)
{
	SetEnumIfPresentAndValid(node, L"Mode", this, CXmlEnums::StringToIDimensionChannelAiryscanSettingsMode, &CDimensionChannelAiryscanSettings::SetMode);
	SetIfPresentAndValid(node, L"VirtualPinholeSize", this, &CDimensionChannelAiryscanSettings::SetVirtualPinholeSize);
	SetIfPresentAndValid(node, L"Magnification", this, &CDimensionChannelAiryscanSettings::SetMagnification);
	SetIfPresentAndValid(node, L"TransformationXX", this, &CDimensionChannelAiryscanSettings::SetTransformationXX);
	SetIfPresentAndValid(node, L"TransformationXY", this, &CDimensionChannelAiryscanSettings::SetTransformationXY);
	SetIfPresentAndValid(node, L"TransformationYX", this, &CDimensionChannelAiryscanSettings::SetTransformationYX);
	SetIfPresentAndValid(node, L"TransformationYY", this, &CDimensionChannelAiryscanSettings::SetTransformationYY);

	auto n = node.child(L"FastAiryScanSettings");
	if (!!n)
	{
		this->fastAiryScanSettings = make_shared<CFastAiryScanSettings>(n);
	}
}

// ----------------------------------------------------------------------------

void CDimensionChannelRatio::ParseInformation(pugi::xml_node node)
{
	SetEnumIfPresentAndValid(node, L"Type", this, CXmlEnums::StringToIDimensionChannelRatioType, &CDimensionChannelRatio::SetType);
	SetIfPresentAndValid(node, L"Const1", this, &CDimensionChannelRatio::SetConst1);
	SetIfPresentAndValid(node, L"Const2", this, &CDimensionChannelRatio::SetConst2);
	SetIfPresentAndValid(node, L"Const3", this, &CDimensionChannelRatio::SetConst3);
	SetIfPresentAndValid(node, L"Const4", this, &CDimensionChannelRatio::SetConst4);
	SetIfPresentAndValid(node, L"Const5", this, &CDimensionChannelRatio::SetConst5);
	SetIfPresentAndValid(node, L"Const6", this, &CDimensionChannelRatio::SetConst6);

	auto n = node.child(L"Source1");
	if (!!n)
	{
		auto cr = CDimensionChannelRatio::ParseChannelRefSequence(n);
		this->SetSource1(cr);
	}

	n = node.child(L"Source2");
	if (!!n)
	{
		auto cr = CDimensionChannelRatio::ParseChannelRefSequence(n);
		this->SetSource2(cr);
	}
}

/*static*/std::vector<std::wstring> CDimensionChannelRatio::ParseChannelRefSequence(pugi::xml_node node)
{
	static const wchar_t* ChannelRefNodeName = L"ChannelRef";
	static const wchar_t* AttributeIdName = L"Id";
	std::vector<std::wstring> list;
	for (pugi::xml_node chref = node.child(ChannelRefNodeName); chref; chref = chref.next_sibling(ChannelRefNodeName))
	{
		auto attr = chref.attribute(AttributeIdName);
		if (!!attr)
		{
			list.push_back(attr.as_string());
		}
	}

	return list;
}

// ----------------------------------------------------------------------------

void CFastAiryScanSettings::ParseInformation(pugi::xml_node node)
{
	auto n = node.child(L"IlluminationLinePixels");
	if (!!n)
	{
		std::vector<double> vec = CXmlEnums::ParseDoubleXmlArray(n.text().as_string());
		if (vec.size() >= 2)
		{
			Vector v{ vec[0],vec[1] };
			this->SetIlluminationLinePixels(v);
		}
	}

	n = node.child(L"IlluminationLine");
	if (!!n)
	{
		std::vector<double> vec = CXmlEnums::ParseDoubleXmlArray(n.text().as_string());
		if (vec.size() >= 2)
		{
			Vector v{ vec[0],vec[1] };
			this->SetIlluminationLine(v);
		}
	}

	n = node.child(L"FiberMapping");
	if (!!n)
	{
		this->SetFiberMapping(CXmlEnums::ParseInt32XmlArray(n.text().as_string()));
	}

	SetIfPresentAndValid(node, L"EffectiveNA_in_x", this, &CFastAiryScanSettings::SetEffectiveNAinX);
	SetIfPresentAndValid(node, L"EffectiveNA_in_y", this, &CFastAiryScanSettings::SetEffectiveNAinY);
}

// ----------------------------------------------------------------------------

void CDimensionChannelPolarizingSettings::ParseInformation(pugi::xml_node node)
{
	SetIfPresentAndValid(node, L"PolarizerAngle", this, &CDimensionChannelPolarizingSettings::SetPolarizerAngle);
	SetIfPresentAndValid(node, L"AnalyzerAngle", this, &CDimensionChannelPolarizingSettings::SetAnalyzerAngle);
}

// ----------------------------------------------------------------------------

/*static*/bool CXmlEnums::StringToDimensionChannelAcquisitionMode(const wchar_t* sz, DimensionChannelAcquisitionMode* mode)
{
	static const ParseEnumHelper<DimensionChannelAcquisitionMode>::EnumValue enumValues[] =
	{
		{ L"WideField",DimensionChannelAcquisitionMode::WideField},
		{ L"LaserScanningConfocalMicroscopy",DimensionChannelAcquisitionMode::LaserScanningConfocalMicroscopy },
		{ L"SpinningDiskConfocal",DimensionChannelAcquisitionMode::SpinningDiskConfocal },
		{ L"SlitScanConfocal",DimensionChannelAcquisitionMode::SlitScanConfocal },
		{ L"MultiPhotonMicroscopy",DimensionChannelAcquisitionMode::MultiPhotonMicroscopy },
		{ L"StructuredIllumination",DimensionChannelAcquisitionMode::StructuredIllumination },
		{ L"SingleMoleculeImaging",DimensionChannelAcquisitionMode::SingleMoleculeImaging },
		{ L"TotalInternalReflection",DimensionChannelAcquisitionMode::TotalInternalReflection },
		{ L"FluorescenceLifetime",DimensionChannelAcquisitionMode::FluorescenceLifetime },
		{ L"SpectralImaging",DimensionChannelAcquisitionMode::SpectralImaging },
		{ L"FluorescenceCorrelationSpectroscopy",DimensionChannelAcquisitionMode::FluorescenceCorrelationSpectroscopy },
		{ L"NearFieldScanningOpticalMicroscopy",DimensionChannelAcquisitionMode::NearFieldScanningOpticalMicroscopy },
		{ L"SecondHarmonicGenerationImaging",DimensionChannelAcquisitionMode::SecondHarmonicGenerationImaging },
		{ L"PALM",DimensionChannelAcquisitionMode::PALM },
		{ L"STORM",DimensionChannelAcquisitionMode::STORM },
		{ L"STED",DimensionChannelAcquisitionMode::STED },
		{ L"TIRF",DimensionChannelAcquisitionMode::TIRF },
		{ L"FSM",DimensionChannelAcquisitionMode::FSM },
		{ L"LCM",DimensionChannelAcquisitionMode::LCM },
		{ L"SPIM",DimensionChannelAcquisitionMode::SPIM },
		{ L"SEM",DimensionChannelAcquisitionMode::SEM },
		{ L"FIB",DimensionChannelAcquisitionMode::FIB },
		{ L"FIB-SEM",DimensionChannelAcquisitionMode::FIB_SEM},
		{ L"ApertureCorrelation",DimensionChannelAcquisitionMode::ApertureCorrelation },
		{ L"Other",DimensionChannelAcquisitionMode::Other }
	};

	return ParseEnumHelper<DimensionChannelAcquisitionMode>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, mode);
}

/*static*/bool CXmlEnums::StringToDimensionChannelIlluminationType(const wchar_t* sz, DimensionChannelIlluminationType* illuminationtype)
{
	static const ParseEnumHelper<DimensionChannelIlluminationType>::EnumValue enumValues[] =
	{
		{L"Transmitted",DimensionChannelIlluminationType::Transmitted},
		{L"Epifluorescence",DimensionChannelIlluminationType::Epifluorescence},
		{L"Oblique",DimensionChannelIlluminationType::Oblique},
		{L"NonLinear",DimensionChannelIlluminationType::NonLinear},
		{L"Other",DimensionChannelIlluminationType::Other}
	};

	return ParseEnumHelper<DimensionChannelIlluminationType>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, illuminationtype);
}

/*static*/bool CXmlEnums::StringToDimensionChannelContrastMethod(const wchar_t* sz, DimensionChannelContrastMethod* contrastmethod)
{
	static const ParseEnumHelper<DimensionChannelContrastMethod>::EnumValue enumValues[] =
	{
		{ L"Brightfield", DimensionChannelContrastMethod::Brightfield},
		{ L"Phase",DimensionChannelContrastMethod::Phase },
		{ L"DIC",DimensionChannelContrastMethod::DIC },
		{ L"HoffmanModulation",DimensionChannelContrastMethod::HoffmanModulation },
		{ L"ObliqueIllumination",DimensionChannelContrastMethod::ObliqueIllumination },
		{ L"PolarizedLight",DimensionChannelContrastMethod::PolarizedLight },
		{ L"Darkfield",DimensionChannelContrastMethod::Darkfield },
		{ L"Fluorescence",DimensionChannelContrastMethod::Fluorescence },
		{ L"MultiPhotonFluorescence",DimensionChannelContrastMethod::MultiPhotonFluorescence },
		{ L"Other",DimensionChannelContrastMethod::Other }
	};

	return ParseEnumHelper<DimensionChannelContrastMethod>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, contrastmethod);
}

/*static*/bool CXmlEnums::StringToDimensionChannelPinholeGeometry(const wchar_t* sz, DimensionChannelPinholeGeometry* pinholegeometry)
{
	static const ParseEnumHelper<DimensionChannelPinholeGeometry>::EnumValue enumValues[] =
	{
		{ L"Circular", DimensionChannelPinholeGeometry::Circular},
		{ L"Rectangular",DimensionChannelPinholeGeometry::Rectangular },
		{ L"Other",DimensionChannelPinholeGeometry::Other }
	};

	return ParseEnumHelper<DimensionChannelPinholeGeometry>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, pinholegeometry);
}

/*static*/bool CXmlEnums::StringToDimensionChannelChannelType(const wchar_t* sz, DimensionChannelChannelType* channeltype)
{
	static const ParseEnumHelper<DimensionChannelChannelType>::EnumValue enumValues[] =
	{
		{ L"Heightmap", DimensionChannelChannelType::Heightmap},
		{ L"PalHR",DimensionChannelChannelType::PalHR },
		{ L"PalWidefield",DimensionChannelChannelType::PalWidefield },
		{ L"SimHR",DimensionChannelChannelType::SimHR },
		{ L"SimWidefield",DimensionChannelChannelType::SimWidefield },
		{ L"SimDWF",DimensionChannelChannelType::SimDWF },
		{ L"AiryScanSum",DimensionChannelChannelType::AiryScanSum },
		{ L"AiryScanRawSr",DimensionChannelChannelType::AiryScanRawSr },
		{ L"AiryScanRaw",DimensionChannelChannelType::AiryScanRaw },
		{ L"AiryScanSr",DimensionChannelChannelType::AiryScanSr },
		{ L"AiryScanVp",DimensionChannelChannelType::AiryScanVp },
		{ L"AiryScanMb",DimensionChannelChannelType::AiryScanMb },
		{ L"AiryScanRingSheppardSum",DimensionChannelChannelType::AiryScanRingSheppardSum },
		{ L"OnlineUnmixing",DimensionChannelChannelType::OnlineUnmixing },
		{ L"Unspecified",DimensionChannelChannelType::Unspecified }
	};

	return ParseEnumHelper<DimensionChannelChannelType>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, channeltype);
}

/*static*/bool CXmlEnums::StringToPixelType(const wchar_t* sz, libCZI::PixelType* pixeltype)
{
	static const ParseEnumHelper<libCZI::PixelType>::EnumValue enumValues[] =
	{
		{L"Gray8",PixelType::Gray8},
		{L"Gray16",PixelType::Gray16},
		{L"Gray32Float",PixelType::Gray32Float},
		{L"Bgr24",PixelType::Bgr24},
		{L"Bgr48",PixelType::Bgr48},
		{L"Bgr96Float",PixelType::Bgr96Float},
		{L"Bgra32",PixelType::Bgra32},
		{L"Gray64ComplexFloat",PixelType::Gray64ComplexFloat},
		{L"Bgr192ComplexFloat",PixelType::Bgr192ComplexFloat},
		{L"Gray32",PixelType::Gray32},
		{L"Gray64Float",PixelType::Gray64Float}
	};

	return ParseEnumHelper<libCZI::PixelType>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, pixeltype);
}

/*static*/bool CXmlEnums::StringToBinning(const wchar_t* sz, libCZI::ChannelDetectorSettingsBinning* binning)
{
	static const ParseEnumHelper<libCZI::ChannelDetectorSettingsBinning>::EnumValue enumValues[] =
	{
		{L"1x1",ChannelDetectorSettingsBinning::_1x1},
		{L"2x2",ChannelDetectorSettingsBinning::_2x2},
		{L"4x4",ChannelDetectorSettingsBinning::_4x4},
		{L"8x8",ChannelDetectorSettingsBinning::_8x8},
		{L"Other",ChannelDetectorSettingsBinning::Other}
	};

	return ParseEnumHelper<libCZI::ChannelDetectorSettingsBinning>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, binning);
}

/*static*/bool CXmlEnums::StringToShutter(const wchar_t* sz, libCZI::ChannelDetectorSettingsShutter* shutter)
{
	static const ParseEnumHelper<libCZI::ChannelDetectorSettingsShutter>::EnumValue enumValues[] =
	{
		{L"Rolling",ChannelDetectorSettingsShutter::Rolling},
		{L"Global",ChannelDetectorSettingsShutter::Global},
		{L"Other",ChannelDetectorSettingsShutter::Other}
	};

	return ParseEnumHelper<libCZI::ChannelDetectorSettingsShutter>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, shutter);
}

/*static*/bool CXmlEnums::StringToCameraProcessingMode(const wchar_t* sz, libCZI::ChannelDetectorCameraProcessingMode* mode)
{
	static const ParseEnumHelper<libCZI::ChannelDetectorCameraProcessingMode>::EnumValue enumValues[] =
	{
		{L"None",ChannelDetectorCameraProcessingMode::None},
		{L"Widefield",ChannelDetectorCameraProcessingMode::Widefield},
		{L"Sectioned",ChannelDetectorCameraProcessingMode::Sectioned},
		{L"RawDataMode",ChannelDetectorCameraProcessingMode::RawDataMode}
	};

	return ParseEnumHelper<libCZI::ChannelDetectorCameraProcessingMode>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, mode);
}

/*static*/bool CXmlEnums::StringToDimensionChannelLaserScanScanningMode(const wchar_t* sz, libCZI::DimensionChannelLaserScanScanningMode* scanningMode)
{
	static const ParseEnumHelper<libCZI::DimensionChannelLaserScanScanningMode>::EnumValue enumValues[] =
	{
		{L"Frame",DimensionChannelLaserScanScanningMode::Frame},
		{L"FastFrame",DimensionChannelLaserScanScanningMode::FastFrame},
		{L"LineSequential",DimensionChannelLaserScanScanningMode::LineSequential},
		{L"Line",DimensionChannelLaserScanScanningMode::Line},
		{L"Spot",DimensionChannelLaserScanScanningMode::Spot},
		{L"Other",DimensionChannelLaserScanScanningMode::Other}
	};

	return ParseEnumHelper<libCZI::DimensionChannelLaserScanScanningMode>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, scanningMode);
}

/*static*/bool CXmlEnums::StringToDimensionChannelLaserScanScanDirection(const wchar_t* sz, libCZI::DimensionChannelLaserScanScanDirection* scanDirection)
{
	static const ParseEnumHelper<libCZI::DimensionChannelLaserScanScanDirection>::EnumValue enumValues[] =
	{
		{L"Bidirectional",DimensionChannelLaserScanScanDirection::Bidirectional},
		{L"Unidirectional",DimensionChannelLaserScanScanDirection::Unidirectional}
	};

	return ParseEnumHelper<libCZI::DimensionChannelLaserScanScanDirection>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, scanDirection);
}

/*static*/bool CXmlEnums::StringToDimensionChannelLaserScanAveragingMethod(const wchar_t* sz, libCZI::DimensionChannelLaserScanAveragingMethod* averagingMethod)
{
	static const ParseEnumHelper<libCZI::DimensionChannelLaserScanAveragingMethod>::EnumValue enumValues[] =
	{
		{L"Mean",DimensionChannelLaserScanAveragingMethod::Mean},
		{L"Sum",DimensionChannelLaserScanAveragingMethod::Sum},
		{L"Other",DimensionChannelLaserScanAveragingMethod::Other}
	};

	return ParseEnumHelper<libCZI::DimensionChannelLaserScanAveragingMethod>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, averagingMethod);
}

/*static*/bool CXmlEnums::StringToDimensionChannelLaserScanAveragingMode(const wchar_t* sz, libCZI::DimensionChannelLaserScanAveragingMode* averagingMode)
{
	static const ParseEnumHelper<libCZI::DimensionChannelLaserScanAveragingMode>::EnumValue enumValues[] =
	{
		{L"Frame",DimensionChannelLaserScanAveragingMode::Frame},
		{L"Line",DimensionChannelLaserScanAveragingMode::Line},
		{L"Other",DimensionChannelLaserScanAveragingMode::Other}
	};

	return ParseEnumHelper<libCZI::DimensionChannelLaserScanAveragingMode>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, averagingMode);
}

/*static*/bool CXmlEnums::StringToDimensionChannelLaserScanLaserAttenuatorState(const wchar_t* sz, libCZI::DimensionChannelLaserScanLaserAttenuatorState* attenuatorState)
{
	static const ParseEnumHelper<libCZI::DimensionChannelLaserScanLaserAttenuatorState>::EnumValue enumValues[] =
	{
		{L"On",DimensionChannelLaserScanLaserAttenuatorState::On},
		{L"Off",DimensionChannelLaserScanLaserAttenuatorState::Off}
	};

	return ParseEnumHelper<libCZI::DimensionChannelLaserScanLaserAttenuatorState>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, attenuatorState);
}

/*static*/bool CXmlEnums::StringToDimensionChannelSPIMIlluminationSettingsBeamType(const wchar_t* sz, libCZI::DimensionChannelSPIMIlluminationSettingsBeamType* beamType)
{
	static const ParseEnumHelper<libCZI::DimensionChannelSPIMIlluminationSettingsBeamType>::EnumValue enumValues[] =
	{
		{L"Gauss",DimensionChannelSPIMIlluminationSettingsBeamType::Gauss},
		{L"Bessel",DimensionChannelSPIMIlluminationSettingsBeamType::Bessel},
		{L"Other",DimensionChannelSPIMIlluminationSettingsBeamType::Other}
	};

	return ParseEnumHelper<libCZI::DimensionChannelSPIMIlluminationSettingsBeamType>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, beamType);
}

/*static*/bool CXmlEnums::StringToIInstrumentObjectiveSettingsMedium(const wchar_t* sz, libCZI::IInstrumentObjectiveSettings::Medium* medium)
{
	static const ParseEnumHelper<libCZI::IInstrumentObjectiveSettings::Medium>::EnumValue enumValues[] =
	{
		{L"Air",IInstrumentObjectiveSettings::Medium::Air},
		{L"Oil",IInstrumentObjectiveSettings::Medium::Oil},
		{L"Water",IInstrumentObjectiveSettings::Medium::Water},
		{L"Glycerol",IInstrumentObjectiveSettings::Medium::Glycerol},
		{L"Other",IInstrumentObjectiveSettings::Medium::Other}
	};

	return ParseEnumHelper<libCZI::IInstrumentObjectiveSettings::Medium>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, medium);
}

/*static*/bool CXmlEnums::StringToIDimensionChannelAiryscanSettingsMode(const wchar_t* sz, libCZI::IDimensionChannelAiryscanSettings::Mode* mode)
{
	static const ParseEnumHelper<libCZI::IDimensionChannelAiryscanSettings::Mode>::EnumValue enumValues[] =
	{
		{L"SuperResolution",IDimensionChannelAiryscanSettings::Mode::SuperResolution},
		{L"VirtualPinhole",IDimensionChannelAiryscanSettings::Mode::VirtualPinhole},
		{L"MultiBeam",IDimensionChannelAiryscanSettings::Mode::MultiBeam},
		{L"Off",IDimensionChannelAiryscanSettings::Mode::Off}
	};

	return ParseEnumHelper<libCZI::IDimensionChannelAiryscanSettings::Mode>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, mode);
}

/*static*/bool CXmlEnums::StringToIDimensionChannelRatioType(const wchar_t* sz, libCZI::IDimensionChannelRatio::Type* type)
{
	static const ParseEnumHelper< libCZI::IDimensionChannelRatio::Type>::EnumValue enumValues[] =
	{
		{L"NoOnlineCalculation",IDimensionChannelRatio::Type::NoOnlineCalculation},
		{L"OnlineRatio",IDimensionChannelRatio::Type::OnlineRatio},
		{L"OnlineSubtraction",IDimensionChannelRatio::Type::OnlineSubtraction},
		{L"OnlineSubtractionRatio",IDimensionChannelRatio::Type::OnlineSubtractionRatio},
		{L"OnlineHillFunction",IDimensionChannelRatio::Type::OnlineHillFunction},
		{L"OnlineReferenceRatio",IDimensionChannelRatio::Type::OnlineReferenceRatio},
		{L"OnlineLinearUnmixing",IDimensionChannelRatio::Type::OnlineLinearUnmixing}
	};

	return ParseEnumHelper< libCZI::IDimensionChannelRatio::Type>::TryParseEnum(enumValues, sizeof(enumValues) / sizeof(enumValues[0]), sz, type);
}

/*static*/std::vector<double> CXmlEnums::ParseDoubleXmlArray(const std::wstring& str)
{
	std::vector<double> vec;
	size_t offset = 0;
	try
	{
		for (; str[offset] != L'\0';)
		{
			size_t s;
			double d = std::stod(str.c_str() + offset, &s);
			if (offset + s >= str.length())
			{
				vec.push_back(d);
				break;
			}

			if (!isspace(str[offset + s]))
			{
				break;
			}

			for (++s; isspace(str[offset + s]); ++s) {};
			offset += s;
			vec.push_back(d);
		}
	}
	catch (invalid_argument&)
	{
	}
	catch (out_of_range&)
	{
	}

	return vec;
}

/*static*/std::vector<int> CXmlEnums::ParseInt32XmlArray(const std::wstring& str)
{
	std::vector<int> vec;

	size_t offset = 0;
	try
	{
		for (; str[offset] != L'\0';)
		{
			size_t s;
			int d = std::stoi(str.c_str() + offset, &s);
			if (offset + s >= str.length())
			{
				vec.push_back(d);
				break;
			}

			if (!isspace(str[offset + s]))
			{
				break;
			}

			for (++s; isspace(str[offset + s]); ++s) {};
			offset += s;
			vec.push_back(d);
		}
	}
	catch (invalid_argument&)
	{
	}
	catch (out_of_range&)
	{
	}

	return vec;
}