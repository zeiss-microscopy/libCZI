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
#include <cctype>
#include "CziDisplaySettings.h"
#include "splines.h"
#include "utilities.h"
#include "Site.h"

using namespace std;
using namespace libCZI;

class CCziDisplaySettingsUtils
{
public:
		static std::vector<libCZI::IDisplaySettings::SplineControlPoint> ParseSplinePoints(const wchar_t* szString)
	{
		// example for syntax: 0.362559241706161,0.876190476190476 0.554502369668246,0.561904761904762

		// TODO: parsing not really watertight I suppose...
		std::vector<libCZI::IDisplaySettings::SplineControlPoint> splinePts;
		size_t charsParsed;
		for (; *szString != L'\0';)
		{
			double x, y;
			x = std::stof(szString, &charsParsed);
			szString += charsParsed;

			// now, skip whitespace and exactly one comma
			for (;;)
			{
				wchar_t c = *szString;
				if (std::isspace(c))
				{
					++szString;
					continue;
				}

				if (c == L',')
				{
					++szString;
					break;
				}

				throw std::invalid_argument("invalid syntax");
			}

			y = std::stof(szString, &charsParsed);
			szString += charsParsed;

			splinePts.push_back({ x,y });

			for (;;)
			{
				wchar_t c = *szString;
				if (std::isspace(c))
				{
					++szString;
					continue;
				}

				break;
			}
		}

		return splinePts;
	}

	static void GetSplineDataFromSplineControlPoints(const std::vector<libCZI::IDisplaySettings::SplineControlPoint> & splineControlPoints, std::vector<libCZI::IDisplaySettings::SplineData> & data)
	{
		auto coeffs = CSplines::GetSplineCoefficients(
			(int)splineControlPoints.size() + 2,
			[&](int index, double* px, double* py)->void
		{
			if (index == 0)
			{
				if (px != nullptr) { *px = 0; }
				if (py != nullptr) { *py = 0; }
			}
			else if (index == splineControlPoints.size() + 1)
			{
				if (px != nullptr) { *px = 1; }
				if (py != nullptr) { *py = 1; }
			}
			else
			{
				if (px != nullptr) { *px = splineControlPoints[index - 1].x; }
				if (py != nullptr) { *py = splineControlPoints[index - 1].y; }
			}
		});

		std::vector<libCZI::IDisplaySettings::SplineData> splineData; splineData.reserve(coeffs.size());

		data.clear();
		data.reserve(coeffs.size());
		for (size_t i = 0; i < coeffs.size(); ++i)
		{
			IDisplaySettings::SplineData spD{ i == 0 ? 0 : splineControlPoints.at(i - 1).x ,coeffs.at(i) };
			data.push_back(spD);
		}
	}
};

/*static*/void IDisplaySettings::Clone(const IDisplaySettings* disp, DisplaySettingsPOD& pod)
{
	pod.channelDisplaySettings.clear();
	disp->EnumChannels(
		[&](int chIdx)->bool
	{
		auto chDisplaySetting = disp->GetChannelDisplaySettings(chIdx);
		IChannelDisplaySetting::Clone(chDisplaySetting.get(), pod.channelDisplaySettings[chIdx]);
		return true;
	});
}

/*static*/void IChannelDisplaySetting::Clone(const libCZI::IChannelDisplaySetting* disp, libCZI::ChannelDisplaySettingsPOD& pod)
{
	pod.isEnabled = disp->GetIsEnabled();
	pod.weight = disp->GetWeight();
	pod.gradationCurveMode = disp->GetGradationCurveMode();
	disp->GetBlackWhitePoint(&pod.blackPoint, &pod.whitePoint);
	switch (pod.gradationCurveMode)
	{
	case IDisplaySettings::GradationCurveMode::Linear:
		break;
	case IDisplaySettings::GradationCurveMode::Gamma:
		if (!disp->TryGetGamma(&pod.gamma))
		{
			pod.gamma = 1;
		}

		break;
	case IDisplaySettings::GradationCurveMode::Spline:
		if (!disp->TryGetSplineControlPoints(&pod.splineCtrlPoints))
		{
			pod.splineCtrlPoints.clear();
			pod.splineCtrlPoints.push_back(IDisplaySettings::SplineControlPoint{ 0, 0 });
			pod.splineCtrlPoints.push_back(IDisplaySettings::SplineControlPoint{ 1, 1 });
		}

		break;
	default:
		{
			stringstream ss;
			ss << "Invalid value for \"gradation-curve-mode\" encountered (" << (underlying_type<IDisplaySettings::GradationCurveMode>::type)(pod.gradationCurveMode) << ")";
			throw invalid_argument(ss.str());
		}

		break;
	}

	if (disp->TryGetTintingColorRgb8(&pod.tintingColor))
	{
		pod.tintingMode = IDisplaySettings::TintingMode::Color;
	}
	else
	{
		pod.tintingMode = IDisplaySettings::TintingMode::None;
	}
}

//-----------------------------------------------------------------------------

//*****************************************************************************

bool CChannelDisplaySettingsOnPod::GetIsEnabled() const /*override*/
{
	return this->cdsPod.isEnabled;
}

float CChannelDisplaySettingsOnPod::GetWeight() const /*override*/
{
	return this->cdsPod.weight;
}

bool CChannelDisplaySettingsOnPod::TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const /*override*/
{
	if (this->cdsPod.tintingMode == IDisplaySettings::TintingMode::Color)
	{
		if (pColor != nullptr)
		{
			*pColor = this->cdsPod.tintingColor;
		}

		return true;
	}

	return false;
}

void CChannelDisplaySettingsOnPod::GetBlackWhitePoint(float* pBlack, float* pWhite) const /*override*/
{
	if (pBlack != nullptr)
	{
		*pBlack = this->cdsPod.blackPoint;
	}

	if (pWhite != nullptr)
	{
		*pWhite = this->cdsPod.whitePoint;
	}
}

libCZI::IDisplaySettings::GradationCurveMode CChannelDisplaySettingsOnPod::GetGradationCurveMode() const /*override*/
{
	return this->cdsPod.gradationCurveMode;
}

bool CChannelDisplaySettingsOnPod::TryGetGamma(float* gamma) const /*override*/
{
	if (this->cdsPod.gradationCurveMode == IDisplaySettings::GradationCurveMode::Gamma)
	{
		if (gamma != nullptr)
		{
			*gamma = this->cdsPod.gamma;
		}

		return true;
	}

	return false;
}

bool CChannelDisplaySettingsOnPod::TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const /*override*/
{
	if (this->cdsPod.gradationCurveMode == IDisplaySettings::GradationCurveMode::Spline)
	{
		if (ctrlPts != nullptr)
		{
			*ctrlPts = this->cdsPod.splineCtrlPoints;
		}

		return true;
	}

	return false;
}

bool CChannelDisplaySettingsOnPod::TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const /*override*/
{
	if (this->cdsPod.gradationCurveMode == IDisplaySettings::GradationCurveMode::Spline)
	{
		if (data != nullptr)
		{
			CCziDisplaySettingsUtils::GetSplineDataFromSplineControlPoints(this->cdsPod.splineCtrlPoints, *data);
		}

		return true;
	}

	return false;
}

//*****************************************************************************

/*static*/IChannelDisplaySetting* ChannelDisplaySettingsPOD::CreateIChannelDisplaySetting(const libCZI::ChannelDisplaySettingsPOD& pod)
{
	return new CChannelDisplaySettingsOnPod(pod);
}

/*static*/std::shared_ptr<libCZI::IChannelDisplaySetting> ChannelDisplaySettingsPOD::CreateIChannelDisplaySettingSp(const ChannelDisplaySettingsPOD & pod)
{
	return std::make_shared<CChannelDisplaySettingsOnPod>(pod);
}

/*static*/std::shared_ptr<libCZI::IDisplaySettings> CDisplaySettingsOnPod::CreateFromXml(pugi::xml_node node)
{
	auto channels = node.child(L"Channels");
	if (channels.empty())
	{
		return std::shared_ptr<libCZI::IDisplaySettings>();
	}

	int channelIdx = 0;
	vector<tuple<int, ChannelDisplaySettingsPOD>> displSettings;
	for (auto ch : channels.children(L"Channel"))
	{
		ChannelDisplaySettingsPOD chDsplSetting;
		chDsplSetting.gradationCurveMode = GradationCurveMode::Linear;
		auto subNode = ch.child(L"Low");
		if (!subNode.empty())
		{
			chDsplSetting.blackPoint = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.blackPoint = 0;
		}

		subNode = ch.child(L"High");
		if (!subNode.empty())
		{
			chDsplSetting.whitePoint = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.whitePoint = 1;
		}

		subNode = ch.child(L"Weight");
		if (!subNode.empty())
		{
			chDsplSetting.weight = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.weight = 1;
		}

		chDsplSetting.tintingMode = IDisplaySettings::TintingMode::Color;
		subNode = ch.child(L"ColorMode");
		if (!subNode.empty())
		{
			auto s = Utilities::Trim(subNode.text().as_string());
			if (Utilities::icasecmp(s, L"none"))
			{
				chDsplSetting.tintingMode = IDisplaySettings::TintingMode::None;
			}
		}

		subNode = ch.child(L"Color");
		chDsplSetting.tintingColor.r = chDsplSetting.tintingColor.g = chDsplSetting.tintingColor.b = 0xff;
		if (!subNode.empty())
		{
			bool b = Utilities::TryGetRgb8ColorFromString(subNode.text().as_string(), chDsplSetting.tintingColor);
			if (b == false)
			{
				GetSite()->Log(LOGLEVEL_WARNING, "Could not parse the content of the 'Color'-field ");
			}
		}

		subNode = ch.child(L"IsSelected");
		if (!subNode.empty())
		{
			chDsplSetting.isEnabled = subNode.text().as_bool();
		}
		else
		{
			chDsplSetting.isEnabled = true;
		}

		subNode = ch.child(L"Gamma");
		if (!subNode.empty())
		{
			chDsplSetting.gamma = subNode.text().as_float();
			chDsplSetting.gradationCurveMode = GradationCurveMode::Gamma;
		}
		else
		{
			chDsplSetting.gamma = std::numeric_limits<float>::quiet_NaN();
		}

		subNode = ch.child(L"Mode");
		if (!subNode.empty())
		{
			auto s = Utilities::Trim(subNode.text().as_string());
			if (Utilities::icasecmp(s, L"spline"))
			{
				auto subNodePoints = ch.child(L"Points");
				if (!subNodePoints.empty())
				{
					try
					{
						chDsplSetting.splineCtrlPoints = CCziDisplaySettingsUtils::ParseSplinePoints(subNodePoints.text().as_string());
					}
					catch (invalid_argument&)
					{
						GetSite()->Log(LOGLEVEL_WARNING, "Could not parse the content of the 'Point'-field");
					}

					if (!chDsplSetting.splineCtrlPoints.empty())
					{
						chDsplSetting.gradationCurveMode = GradationCurveMode::Spline;
					}
					else
					{
						GetSite()->Log(LOGLEVEL_WARNING, "The 'Point'-field was found to be empty");
					}
				}
			}
		}

		displSettings.emplace_back(make_tuple(channelIdx, chDsplSetting));
		channelIdx++;

		// TODO: what is authorative? The position of the node, the channel-Id or the channel-Name?
	}

	return std::make_shared<CDisplaySettingsOnPod>(
		[&](int no, int& chIdx, ChannelDisplaySettingsPOD & chDsplSettings) -> bool
	{
		if (no < (int)displSettings.size())
		{
			chIdx = get<0>(displSettings[no]);
			chDsplSettings = get<1>(displSettings[no]);
			return true;
		}

		return false;
	});
}

CDisplaySettingsOnPod::CDisplaySettingsOnPod(const DisplaySettingsPOD& pod)
{
	for (auto it = pod.channelDisplaySettings.cbegin(); it != pod.channelDisplaySettings.cend(); ++it)
	{
		this->channelDs[it->first] = ChannelDisplaySettingsPOD::CreateIChannelDisplaySettingSp(it->second);
	}
}

CDisplaySettingsOnPod::CDisplaySettingsOnPod(std::function<bool(int no, int&, libCZI::ChannelDisplaySettingsPOD& dispSetting)> getChannelDisplaySettings)
{
	if (getChannelDisplaySettings)
	{
		int no = 0;
		for (;; ++no)
		{
			int chIdx; ChannelDisplaySettingsPOD chDsplSettings;
			if (!getChannelDisplaySettings(no, chIdx, chDsplSettings))
			{
				break;
			}

			this->channelDs[chIdx] = ChannelDisplaySettingsPOD::CreateIChannelDisplaySettingSp(chDsplSettings);
		}
	}
}

void CDisplaySettingsOnPod::CDisplaySettingsOnPod::EnumChannels(std::function<bool(int)> func) const /*override*/
{
	for (std::map<int, shared_ptr< IChannelDisplaySetting>>::const_iterator it = this->channelDs.cbegin(); it != this->channelDs.cend(); ++it)
	{
		bool b = func(it->first);
		if (b == false)
		{
			break;
		}
	}
}

std::shared_ptr<libCZI::IChannelDisplaySetting> CDisplaySettingsOnPod::GetChannelDisplaySettings(int chIndex) const /*override*/
{
	const auto& it = this->channelDs.find(chIndex);
	if (it != this->channelDs.cend())
	{
		return it->second;
	}

	return std::shared_ptr<libCZI::IChannelDisplaySetting>();
}

/*static*/IDisplaySettings* DisplaySettingsPOD::CreateIDisplaySetting(const libCZI::DisplaySettingsPOD& pod)
{
	return new CDisplaySettingsOnPod(pod);
}

/*static*/std::shared_ptr<libCZI::IDisplaySettings> DisplaySettingsPOD::CreateIDisplaySettingSp(const libCZI::DisplaySettingsPOD& pod)
{
	return make_shared<CDisplaySettingsOnPod>(pod);
}