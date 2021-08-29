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

#pragma once

#include "libCZI.h"
#include "CziMetadata.h"

class CCziDimensionInfo : public libCZI::IDimensionInfo
{
private:
	std::shared_ptr<CCziMetadata> md;
	libCZI::DimensionAndStartSize dimAndStartSize;
public:
	CCziDimensionInfo(std::shared_ptr<CCziMetadata> md, const libCZI::DimensionAndStartSize& dimAndStartSize)
		: md(md), dimAndStartSize(dimAndStartSize)
	{}

	virtual ~CCziDimensionInfo() = default;
public: // interface IDimensionInfo
	virtual libCZI::DimensionIndex GetDimension() const;
	virtual void GetInterval(int* start, int* end) const;

};

class CCziDimensionZInfo : public libCZI::IDimensionZInfo
{
private:
	enum class DefType : std::uint8_t
	{
		None,
		Interval,
		List
	};

	DefType	type;
	double intervalStart;
	double intervalIncrement;
	std::vector<double> data;
	bool referencePosValid;
	double referencePos;
	bool xyzHandednessValid;
	libCZI::IDimensionZInfo::XyzHandedness xyzHandedness;
	bool zaxisDirectionValid;
	libCZI::IDimensionZInfo::ZaxisDirection zaxisDirection;
	bool zdriveModeValid;
	libCZI::IDimensionZInfo::ZDriveMode zdriveMode;
	bool zdriveSpeedValid;
	double zdriveSpeed;
public:
	CCziDimensionZInfo();

	void SetIntervalDefinition(const double& start, const double& increment);
	void SetListDefinition(std::vector<double>&& list);
	void SetStartPosition(const double& startPos);
	void SetXyzHandedness(libCZI::IDimensionZInfo::XyzHandedness handedness);
	void SetZAxisDirection(libCZI::IDimensionZInfo::ZaxisDirection zaxisDirection);
	void SetZDriveMode(libCZI::IDimensionZInfo::ZDriveMode zdrivemode);
	void SetZDriveSpeed(const double& d);
public:
	bool TryGetReferencePosition(double* d) override;
	bool TryGetIntervalDefinition(double* start, double* increment) override;
	bool TryGetPositionList(std::vector<double>* positions) override;
	bool TryGetXyzHandedness(XyzHandedness* xyzHandedness) override;
	bool TryGetZAxisDirection(ZaxisDirection* zAxisDirection) override;
	bool TryGetZDriveMode(ZDriveMode* zdrivemode) override;
	bool TryZDriveSpeed(double* zdrivespeed) override;
};

class CCziDimensionTInfo : public libCZI::IDimensionTInfo
{
private:
	enum class DefType : std::uint8_t
	{
		None,
		Interval,
		List
	};

	DefType	type;
	double intervalStart;
	double intervalIncrement;
	std::vector<double> offsets;
	bool startTimeValid;
	libCZI::XmlDateTime startTime;
public:
	CCziDimensionTInfo();

	void SetStartTime(const libCZI::XmlDateTime& dateTime);
	void SetIntervalDefinition(const double& start, const double& increment);
	void SetListDefinition(std::vector<double>&& list);
public:
	bool TryGetStartTime(libCZI::XmlDateTime* dateTime) override;
	bool TryGetIntervalDefinition(double* offset, double* increment) override;
	bool TryGetOffsetsList(std::vector<double>* offsets) override;
};