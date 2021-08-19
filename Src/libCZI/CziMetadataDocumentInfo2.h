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
#include "utilities.h"

class CAttributeId : public libCZI::IAttributeId
{
public:
	CAttributeId(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	bool TryGetAttributeId(std::wstring* id) override { return this->attributeId.TryGet(id); }
private:
	Nullable<std::wstring> attributeId;
};

class CAttributeName : public libCZI::IAttributeName
{
public:
	CAttributeName(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	bool TryGetAttributeName(std::wstring* id)override { return this->attributeName.TryGet(id); }
private:
	Nullable<std::wstring> attributeName;
};

class CDimensionChannelInfo : public libCZI::IDimensionChannelInfo
{
public:
	CDimensionChannelInfo(pugi::xml_node node);
public:
	bool TryGetChannelType(libCZI::DimensionChannelChannelType* type) override;
	bool TryGetChannelUnit(std::wstring* unit) override;
	bool TryGetPixelType(libCZI::PixelType* pixelType)override;
	bool TryGetComponentBitCount(int* bitCount)override;
	bool TryGetAcquisitionMode(libCZI::DimensionChannelAcquisitionMode* acquisitionMode)override;
	bool TryGetIlluminationType(libCZI::DimensionChannelIlluminationType* illuminationType)override;
	bool TryGetContrastMethod(libCZI::DimensionChannelContrastMethod* contrastMethod)override;
	bool TryGetIlluminationWavelength(libCZI::SpectrumCharacteristics* illuminationwavelength)override;
	bool TryGetDetectionWavelength(libCZI::SpectrumCharacteristics* detectionwavelength)override;
	bool TryGetExcitationWavelength(double* excitationwavelength) override;
	bool TryGetEmissionWavelength(double* emissionwavelength) override;
	bool TryGetEffectiveNA(double* na) override;
	bool TryGetDyeId(std::wstring* dyeid) override;
	bool TryGetDyeDatabaseId(std::wstring* dyedatabaseid) override;
	bool TryGetPinholeSize(double* pinholesize) override;
	bool TryGetPinholeSizeAiry(double* pinholesizeairy) override;
	bool TryGetPinholeGeometry(libCZI::DimensionChannelPinholeGeometry* pinholegeometry) override;
	bool TryGetFluor(std::wstring* fluor) override;
	bool TryGetNDFilter(double* ndfilter) override;
	bool TryGetPocketCellSetting(int* pocketcellsetting) override;
	bool TryGetColor(libCZI::Rgb8Color* color) override;
	bool TryGetExposureTime(libCZI::RangeOrSingleValue<std::uint64_t>* exposuretime) override;
	bool TryGetDepthOfFocus(double* depthoffocus) override;
	bool TryGetSectionThickness(double* sectionthickness) override;
	std::shared_ptr<libCZI::IDimensionChannelDetectorSettings> GetDetectorSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelLightSourcesSettings> GetLightSourcesSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelLightPath> GetLightPath() override;
	std::shared_ptr<libCZI::IDimensionChannelLaserScanInfo> GetLaserScanInfo() override;
	std::shared_ptr<libCZI::IDimensionChannelSPIMIlluminationSettings> GetSPIMIlluminationSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelSPIMDetectionSettings> GetSPIMDetectionSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelSIMSettings> GetSIMSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelPolarizingSettings> GetPolarizingSettings() override;
	std::shared_ptr<libCZI::IDimensionChannelAiryscanSettings> GetAiryscanSettings() override;
	bool TryGetReflector(std::wstring* reflector) override;
	bool TryGetCondenserContrast(std::wstring* condensorContrast) override;
	bool TryGetNACondenser(double* naCondensor) override;
	std::shared_ptr<libCZI::IDimensionChannelRatio> GetRatio() override;

	bool TryGetAttributeId(std::wstring* id) override { return this->attributeId.TryGetAttributeId(id); }
	bool TryGetAttributeName(std::wstring* id) override { return this->attributeName.TryGetAttributeName(id); }
public:
	void SetChannelType(libCZI::DimensionChannelChannelType type);
	void SetChannelUnit(const std::wstring& unit);
	void SetPixelType(libCZI::PixelType pixelType);
	void SetComponentBitCount(int bitCount);
	void SetAcquisitionMode(libCZI::DimensionChannelAcquisitionMode acquisitionMode);
	void SetIlluminationType(libCZI::DimensionChannelIlluminationType illuminationType);
	void SetContrastMethod(libCZI::DimensionChannelContrastMethod contrastMethod);
	void SetIlluminationWavelength(const libCZI::SpectrumCharacteristics& illuminationwavelength);
	void SetDetectionWavelength(const libCZI::SpectrumCharacteristics& detectionwavelength);
	void SetExcitationWavelength(double excitationwavelength);
	void SetEmissionWavelength(double emissionwavelength);

	void SetEffectiveNA(double effectiveNA);
	void SetDyeId(const std::wstring& dyeid);
	void SetDyeDatabaseId(const std::wstring& dyeid);
	void SetPinholeSize(double pinholeSize);
	void SetPinholeSizeAiry(double pinholeSizeAiry);
	void SetPinholeGeometry(libCZI::DimensionChannelPinholeGeometry pinholeGeometry);
	void SetFluor(const std::wstring& fluor);
	void SetNDFilter(double ndfilter);
	void SetPockelCellSetting(int pocketcellsetting);
	void SetColor(const libCZI::Rgb8Color& color);
	void SetExposureTime(const libCZI::RangeOrSingleValue<std::uint64_t>& exposuretime);
	void SetDepthOfFocus(double depthoffocus);
	void SetSectionThickness(double sectionthickness);
	void SetReflector(const std::wstring& reflector);
	void SetCondenserContrast(const std::wstring& condensorContrast);
	void SetNACondenser(double naCondensor);
private:
	void ParseInformation(pugi::xml_node node);

	static bool TryParseSpectrumCharacteristic(pugi::xml_node node, libCZI::SpectrumCharacteristics* spectrumCharacteristic);
	static bool TryParseRangeOrSingleValue(const std::wstring& str, libCZI::RangeOrSingleValue<double>* rangeOrSingleValue);
	static bool TryParseRangeOrSingleValue(const std::wstring& str, libCZI::RangeOrSingleValue<std::uint64_t>* rangeOrSingleValue);
private:
	CAttributeId attributeId;
	CAttributeName attributeName;
	// data for "IDimensionChannelInfo"
	Nullable<libCZI::DimensionChannelChannelType> channeltype;
	Nullable<std::wstring> channelunit;
	Nullable<libCZI::PixelType> pixeltype;
	Nullable<int> bitcount;
	Nullable< libCZI::DimensionChannelAcquisitionMode> acquisitionMode;
	Nullable<libCZI::DimensionChannelIlluminationType> illuminationType;
	Nullable<libCZI::DimensionChannelContrastMethod> contrastMethod;
	Nullable<libCZI::SpectrumCharacteristics> illuminationWavelength;
	Nullable<libCZI::SpectrumCharacteristics> detectionWavelength;
	Nullable<double> excitationWavelength;
	Nullable<double> emissionWavelength;
	Nullable<double> effectiveNA;
	Nullable<std::wstring> dyeId;
	Nullable<std::wstring> dyeDatabaseId;
	Nullable<double> pinholeSize;
	Nullable<double> pinholeSizeAiry;
	Nullable<libCZI::DimensionChannelPinholeGeometry> pinholeGeometry;
	Nullable<std::wstring> fluor;
	Nullable<double> ndFilter;
	Nullable<int> pocketCellSetting;
	Nullable<libCZI::Rgb8Color> color;
	Nullable<libCZI::RangeOrSingleValue<std::uint64_t>> exposureTime;
	Nullable<double> depthOfFocus;
	Nullable<double> sectionThickness;
	std::shared_ptr<libCZI::IDimensionChannelDetectorSettings> detectorSettings;
	std::shared_ptr<libCZI::IDimensionChannelLightSourcesSettings> lightSourcesSettings;
	std::shared_ptr<libCZI::IDimensionChannelLightPath> lighPath;
	std::shared_ptr<libCZI::IDimensionChannelLaserScanInfo> laserScanInfo;
	std::shared_ptr<libCZI::IDimensionChannelSPIMIlluminationSettings> spimIlluminationSettings;
	std::shared_ptr<libCZI::IDimensionChannelSPIMDetectionSettings> spimDetectionSettings;
	std::shared_ptr<libCZI::IDimensionChannelSIMSettings> simSettings;
	std::shared_ptr<libCZI::IDimensionChannelPolarizingSettings> polarizingSettings;
	std::shared_ptr<libCZI::IDimensionChannelAiryscanSettings> airyscanSettings;
	Nullable<std::wstring> reflector;
	Nullable<std::wstring> condensorContrast;
	Nullable<double> naCondensor;
	std::shared_ptr<libCZI::IDimensionChannelRatio> ratio;
};

class CDimensionsChannelsInfo : public libCZI::IDimensionsChannelsInfo
{
private:
	std::vector<std::shared_ptr<CDimensionChannelInfo>> channels;
public:
	CDimensionsChannelsInfo(pugi::xml_node node);
	static std::shared_ptr<CDimensionsChannelsInfo> TryParse(const CCziMetadata* md);
public:
	int GetChannelCount() const override;
	std::shared_ptr<libCZI::IDimensionChannelInfo> GetChannel(int i) const override;

private:
	void ParseInformation(pugi::xml_node node);
};

class CDimensionChannelDetectorSettings : public libCZI::IDimensionChannelDetectorSettings
{
public:
	CDimensionChannelDetectorSettings(pugi::xml_node node);

public:
	bool TryGetDetectorRef(std::wstring* detectorRef) override;
	bool TryGetBinning(libCZI::ChannelDetectorSettingsBinning* binning) override;
	bool TryGetShutter(libCZI::ChannelDetectorSettingsShutter* shutter) override;
	bool TryGetPhotonConversionFactor(double* conversionFactor) override;
	bool TryGetGain(double* gain) override;
	bool TryGetDigitalGain(double* digitalgain) override;
	bool TryGetOffset(double* offset) override;
	bool TryGetEMGain(double* emgain) override;
	bool TryGetVoltage(double* voltage) override;
	bool TryGetReadOutRate(double* readoutrate) override;
	bool TryGetUseBrightnessContrastCorrection(bool* usebrightnesscontrastcorrection) override;
	bool TryGetPixelMaximum(double* pixelmaximum) override;
	bool TryGetPixelAccuracy(double* pixelaccuracy) override;
	bool TryGetPixelScaleFactor(double* pixelscalefactor) override;
	bool TryGetCameraProcessingMode(libCZI::ChannelDetectorCameraProcessingMode* mode) override;
private:
	void ParseInformation(pugi::xml_node node);

public:
	void SetDetectorRef(const std::wstring& detectorref) { this->detectorRef.Set(detectorref); }
	void SetBinning(libCZI::ChannelDetectorSettingsBinning binning) { this->binning.Set(binning); }
	void SetShutter(libCZI::ChannelDetectorSettingsShutter shutter) { this->shutter.Set(shutter); }
	void SetPhotonConversionFactor(double photonconversionfactor) { this->photonConversionFactor.Set(photonconversionfactor); }
	void SetGain(double gain) { this->gain.Set(gain); }
	void SetDigitalGain(double digitalgain) { this->digitalGain.Set(digitalgain); }
	void SetOffset(double offset) { this->offset.Set(offset); }
	void SetEmGain(double emGain) { this->emGain.Set(emGain); }
	void SetVoltage(double voltage) { this->voltage.Set(voltage); }
	void SetReadOutRate(double readoutrate) { this->readOutRate.Set(readoutrate); }
	void SetUseBrightnessContrastCorrection(bool usebrightnesscontrastcorrection) { this->useBrightnessContrastCorrection.Set(usebrightnesscontrastcorrection); }
	void SetPixelMaximum(double pixelmaximum) { this->pixelMaximum.Set(pixelmaximum); }
	void SetPixelAccuracy(double pixelaccuracy) { this->pixelAccuracy.Set(pixelaccuracy); }
	void SetPixelScaleFactor(double scalefactor) { this->pixelScaleFactor.Set(scalefactor); }
	void SetCameraProcessingMode(libCZI::ChannelDetectorCameraProcessingMode mode) { this->cameraProcessingMode.Set(mode); }
private:
	// data for "IDimensionChannelDetectorSettings"
	Nullable<std::wstring> detectorRef;
	Nullable<libCZI::ChannelDetectorSettingsBinning> binning;
	Nullable<libCZI::ChannelDetectorSettingsShutter> shutter;
	Nullable<double> photonConversionFactor;
	Nullable<double> gain;
	Nullable<double> digitalGain;
	Nullable<double> offset;
	Nullable<double> emGain;
	Nullable<double> voltage;
	Nullable<double> readOutRate;
	Nullable<bool> useBrightnessContrastCorrection;
	Nullable<double> pixelMaximum;
	Nullable<double> pixelAccuracy;
	Nullable<double> pixelScaleFactor;
	Nullable<libCZI::ChannelDetectorCameraProcessingMode> cameraProcessingMode;
};

class CDimensionChannelLightSourcesSetting : public libCZI::IDimensionChannelLightSourcesSetting
{
public:
	CDimensionChannelLightSourcesSetting(pugi::xml_node node);

public:
	bool TryGetLightSourceRef(std::wstring* lightsourceref)override;
	bool TryGetWavelength(double* wavelength) override;
	bool TryGetAttenuation(double* attenuation) override;
	bool TryGetTransmission(double* transmission) override;
	bool TryGetIntensity(std::wstring* intensity) override;
	bool TryGetFlashDuration(double* flashduration) override;
	bool TryGetTIRFAngle(double* tirfangle) override;
	bool TryGetTIRFDepthOfPenetration(double* tirfdepthofpenetration) override;
	bool TryGetIsTIRFCalibrated(bool* isTIRFcalibrated) override;
	bool TryGetIsNlo(bool* isnlo) override;
	bool TryGetPolarization(Polarization* polarization) override;
private:
	void ParseInformation(pugi::xml_node node);
	static void Parse(pugi::xml_node node, Polarization& polarization);
public:
	void SetLightSourceRef(const std::wstring& lightsourceref) { this->lightSourceRef.Set(lightsourceref); }
	void SetWavelength(double wavelength) { this->waveLength.Set(wavelength); }
	void SetAttenuation(double attenuation) { this->attenuation.Set(attenuation); }
	void SetTransmission(double transmission) { this->transmission.Set(transmission); }
	void SetIntensity(const std::wstring& intensity) { this->intensity.Set(intensity); }
	void SetFlashDuration(double flashduration) { this->flashDuration.Set(flashduration); }
	void SetTIRFAngle(double tirfangle) { this->tirfAngle.Set(tirfangle); }
	void SetTIRFDepthOfPenetration(double tirfdepthofpenetration) { this->tirfDepthOfPenetration.Set(tirfdepthofpenetration); }
	void SetIsTIRFCalibrated(bool isTIRFcalibrated) { this->isTIRFCalibrated.Set(isTIRFcalibrated); }
	void SetIsNlo(bool isnlo) { this->isNLO.Set(isnlo); }
	void SetPolarization(const Polarization& polarization) { this->polarization.Set(polarization); }
private:
	Nullable<std::wstring> lightSourceRef;
	Nullable<double> waveLength;
	Nullable<double> attenuation;
	Nullable<double> transmission;
	Nullable<std::wstring> intensity;
	Nullable<double> flashDuration;
	Nullable<double> tirfAngle;
	Nullable<double> tirfDepthOfPenetration;
	Nullable<bool> isTIRFCalibrated;
	Nullable<bool> isNLO;
	Nullable<Polarization> polarization;
};

class CDimensionChannelLightSourcesSettings : public libCZI::IDimensionChannelLightSourcesSettings
{
private:
	std::vector<std::shared_ptr<CDimensionChannelLightSourcesSetting>> lightSourceSettings;
public:
	CDimensionChannelLightSourcesSettings(pugi::xml_node node);
public:
	int GetLightSourceSettingCount() const override;
	std::shared_ptr<libCZI::IDimensionChannelLightSourcesSetting> GetLightSourceSetting(int i) const override;

private:
	void ParseInformation(pugi::xml_node node);
};

class CDimensionChannelLightPath : public libCZI::IDimensionChannelLightPath
{
public:
	CDimensionChannelLightPath(pugi::xml_node node);
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetExcitationFilterRef(const std::wstring& excitationfilterref) { this->excitationFilterRef.Set(excitationfilterref); }
	void SetDichroicFilterRef(const std::wstring& dichroicref) { this->dichroicRef.Set(dichroicref); }
	void SetEmissionFilterRef(const std::wstring& emissionfilterref) { this->emissionFilterRef.Set(emissionfilterref); }
public:
	bool TryGetExcitationFilterRef(std::wstring* excitationFilterRef) override;
	bool TryGetDichroicFilterRef(std::wstring* dichroicRef) override;
	bool TryGetEmissionFilterRef(std::wstring* emissionFilterRef) override;
private:
	Nullable<std::wstring> excitationFilterRef;
	Nullable<std::wstring> dichroicRef;
	Nullable<std::wstring> emissionFilterRef;
};

class CDimensionChannelLaserScanInfo : public libCZI::IDimensionChannelLaserScanInfo
{
public:
	CDimensionChannelLaserScanInfo(pugi::xml_node node);
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetPixelTime(double pixelTime) { this->pixelTime.Set(pixelTime); }
	void SetLineTime(double lineTime) { this->lineTime.Set(lineTime); }
	void SetFrameTime(double frameTime) { this->frameTime.Set(frameTime); }
	void SetAveraging(int averaging) { this->averaging.Set(averaging); }
	void SetScanningMode(libCZI::DimensionChannelLaserScanScanningMode mode) { this->laserScanningMode.Set(mode); }
	void SetSampleRotation(double sampleRotation) { this->sampleRotation.Set(sampleRotation); }
	void SetSampleOffsetX(double sampleOffsetX) { this->sampleOffsetX.Set(sampleOffsetX); }
	void SetSampleOffsetY(double sampleOffsetY) { this->sampleOffsetY.Set(sampleOffsetY); }
	void SetScanDirection(libCZI::DimensionChannelLaserScanScanDirection scanDirection) { this->scanDirection.Set(scanDirection); }
	void SetZoomX(double zoomX) { this->zoomX.Set(zoomX); }
	void SetZoomY(double zoomY) { this->zoomY.Set(zoomY); }
	void SetZoomZ(double zoomZ) { this->zoomZ.Set(zoomZ); }
	void SetCropOffset(double cropOffset) { this->cropOffset.Set(cropOffset); }
	void SetLaserAttenuatorMeas(double laserAttenuatorMeas) { this->laserAttenuatorMeas.Set(laserAttenuatorMeas); }
	void SetLaserBlanking(double laserBlanking) { this->laserBlanking.Set(laserBlanking); }
	void SetLaserAttenuatorBleach(double laserAttenuatorBleack) { this->laserAttenuatorBleack.Set(laserAttenuatorBleack); }
	void SetLineStep(std::uint32_t lineStep) { this->lineStep.Set(lineStep); }
	void SetScanSpeed(int scanSpeed) { this->scanSpeed.Set(scanSpeed); }
	void SetAveragingMethod(libCZI::DimensionChannelLaserScanAveragingMethod averagingMethod) { this->averagingMethod.Set(averagingMethod); }
	void SetAveragingMode(libCZI::DimensionChannelLaserScanAveragingMode averagingMode) { this->averagingMode.Set(averagingMode); }
	void SetLaserAttenuatorState(libCZI::DimensionChannelLaserScanLaserAttenuatorState attenuatorstate) { this->attenuatorState.Set(attenuatorstate); }
public:
	bool TryGetPixelTime(double* pixelTime) override;
	bool TryGetLineTime(double* lineTime) override;
	bool TryGetFrameTime(double* frameTime) override;
	bool TryGetAveraging(int* averaging) override;
	bool TryGetScanningMode(libCZI::DimensionChannelLaserScanScanningMode* mode) override;
	bool TryGetSampleRotation(double* sampleRotation) override;
	bool TryGetSampleOffsetX(double* sampleOffsetX) override;
	bool TryGetSampleOffsetY(double* sampleOffsetY) override;
	bool TryGetScanDirection(libCZI::DimensionChannelLaserScanScanDirection* scanDirection) override;
	bool TryGetZoomX(double* zoomX) override;
	bool TryGetZoomY(double* zoomY) override;
	bool TryGetZoomZ(double* zoomZ) override;
	bool TryGetCropOffset(double* cropOffset) override;
	bool TryGetLaserAttenuatorMeas(double* laserAttenuatorMeas) override;
	bool TryGetLaserBlanking(double* laserBlanking) override;
	bool TryGetLaserAttenuatorBleach(double* laserAttenuatorBleach) override;
	bool TryGetLineStep(std::uint32_t* lineStep) override;
	bool TryGetScanSpeed(int* scanSpeed) override;
	bool TryGetAveragingMethod(libCZI::DimensionChannelLaserScanAveragingMethod* averagingMethod) override;
	bool TryGetAveragingMode(libCZI::DimensionChannelLaserScanAveragingMode* averagingMode) override;
	bool TryGetLaserAttenuatorState(libCZI::DimensionChannelLaserScanLaserAttenuatorState* attenuatorState) override;
private:
	Nullable<double> pixelTime;
	Nullable<double> lineTime;
	Nullable<double> frameTime;
	Nullable<int> averaging;
	Nullable<libCZI::DimensionChannelLaserScanScanningMode> laserScanningMode;
	Nullable<double> sampleRotation;
	Nullable<double> sampleOffsetX;
	Nullable<double> sampleOffsetY;
	Nullable<libCZI::DimensionChannelLaserScanScanDirection> scanDirection;
	Nullable<double> zoomX;
	Nullable<double> zoomY;
	Nullable<double> zoomZ;
	Nullable<double> cropOffset;
	Nullable<double> laserAttenuatorMeas;
	Nullable<double> laserBlanking;
	Nullable<double> laserAttenuatorBleack;
	Nullable<std::uint32_t> lineStep;
	Nullable<int> scanSpeed;
	Nullable<libCZI::DimensionChannelLaserScanAveragingMethod> averagingMethod;
	Nullable<libCZI::DimensionChannelLaserScanAveragingMode> averagingMode;
	Nullable<libCZI::DimensionChannelLaserScanLaserAttenuatorState> attenuatorState;
};

class CDimensionChannelSPIMIlluminationSettings : public libCZI::IDimensionChannelSPIMIlluminationSettings
{
public:
	CDimensionChannelSPIMIlluminationSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetNALightSheet(double nalightsheet) { this->naLightSheet.Set(nalightsheet); }
	void SetBeamType(libCZI::DimensionChannelSPIMIlluminationSettingsBeamType beamtype) { this->beamType.Set(beamtype); }
	void SetCylinderLensRef(const std::wstring& cylinderlensref) { this->cylinderLensRef.Set(cylinderlensref); }
public:
	bool TryGetNALightSheet(double* nalightsheet) override { return this->naLightSheet.TryGet(nalightsheet); }
	bool TryGetBeamType(libCZI::DimensionChannelSPIMIlluminationSettingsBeamType* beamtype) override { return this->beamType.TryGet(beamtype); }
	bool TryGetCylinderLensRef(std::wstring* cylinderlensref) override { return this->cylinderLensRef.TryGet(cylinderlensref); }
	std::shared_ptr<libCZI::IInstrumentObjectiveSettings> GetIlluminationObjectiveSettings() override { return this->illuminationObjectiveSettings; }
	std::shared_ptr<libCZI::ISPIMIlluminationResonanceScannerSettings> GetResonanceScannerSettings() override { return this->illuminationResonanceScannerSettings; }
	std::shared_ptr<libCZI::ISPIMIlluminationGalvoScannerSettings> GetGalvoScannerSettings() override { return this->illuminationGalvoScannerSettings; }
private:
	Nullable<double> naLightSheet;
	Nullable<libCZI::DimensionChannelSPIMIlluminationSettingsBeamType> beamType;
	Nullable<std::wstring> cylinderLensRef;
	std::shared_ptr<libCZI::IInstrumentObjectiveSettings> illuminationObjectiveSettings;
	std::shared_ptr<libCZI::ISPIMIlluminationResonanceScannerSettings> illuminationResonanceScannerSettings;
	std::shared_ptr<libCZI::ISPIMIlluminationGalvoScannerSettings> illuminationGalvoScannerSettings;
};

class CInstrumentObjectiveSettings : public libCZI::IInstrumentObjectiveSettings
{
public:
	CInstrumentObjectiveSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetObjectiveRef(const std::wstring& objectiveRef) { this->objectiveRef.Set(objectiveRef); }
	void SetMedium(libCZI::IInstrumentObjectiveSettings::Medium medium) { this->medium.Set(medium); }
	void SetCorrectionCollar(double correctioncollar) { this->correctioncollar.Set(correctioncollar); }
	void SetCorrectionCollar2(double correctioncollar2) { this->correctioncollar2.Set(correctioncollar2); }
	void SetCorrectionControllerMode(const std::wstring& correctioncontrollermode) { this->correctionControllerMode.Set(correctioncontrollermode); }
	void SetCorrectionControllerBottomThickness(double correctioncontrollerbottomthickness) { this->correctionControllerBottomThickness.Set(correctioncontrollerbottomthickness); }
	void SetCorrectionControllerImagingDepth(double correctioncontrollerimagingdepth) { this->correctionControllerImagingDepth.Set(correctioncontrollerimagingdepth); }
	void SetRefractiveIndex(double refractiveindex) { this->refractiveIndex.Set(refractiveindex); }
public:
	bool TryGetObjectiveRef(std::wstring* objectiveRef) override { return this->objectiveRef.TryGet(objectiveRef); }
	bool TryGetMedium(libCZI::IInstrumentObjectiveSettings::Medium* medium) override { return this->medium.TryGet(medium); }
	bool TryGetCorrectionCollar(double* correctioncollar) override { return  this->correctioncollar.TryGet(correctioncollar); }
	bool TryGetCorrectionCollar2(double* correctioncollar2) override { return  this->correctioncollar2.TryGet(correctioncollar2); }
	bool TryGetCorrectionControllerMode(std::wstring* correctioncontrollermode) override { return  this->correctionControllerMode.TryGet(correctioncontrollermode); }
	bool TryGetCorrectionControllerBottomThickness(double* correctioncontrollerbottomthickness) override { return this->correctionControllerBottomThickness.TryGet(correctioncontrollerbottomthickness); }
	bool TryGetCorrectionControllerImagingDepth(double* correctioncontrollerimagingdepth) override { return this->correctionControllerImagingDepth.TryGet(correctioncontrollerimagingdepth); }
	bool TryGetRefractiveIndex(double* refractiveindex) override { return this->refractiveIndex.TryGet(refractiveindex); }
private:
	Nullable<std::wstring> objectiveRef;
	Nullable<libCZI::IInstrumentObjectiveSettings::Medium> medium;
	Nullable<double> correctioncollar;
	Nullable<double> correctioncollar2;
	Nullable<std::wstring> correctionControllerMode;
	Nullable<double> correctionControllerBottomThickness;
	Nullable<double> correctionControllerImagingDepth;
	Nullable<double> refractiveIndex;
};

class CSPIMIlluminationResonanceScannerSettings :public libCZI::ISPIMIlluminationResonanceScannerSettings
{
public:
	CSPIMIlluminationResonanceScannerSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetAmplitude(double amplitude) { this->amplitude.Set(amplitude); }
public:
	bool TryGetAmplitude(double* amplitude) override { return this->amplitude.TryGet(amplitude); }
private:
	Nullable<double> amplitude;
};

class CSPIMIlluminationGalvoScannerSettings :public libCZI::ISPIMIlluminationGalvoScannerSettings
{
public:
	CSPIMIlluminationGalvoScannerSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetAmplitudeYScan(double amplitudeyscan) { this->amplitudeYScan.Set(amplitudeyscan); }
	void SetFrequency(double frequency) { this->frequency.Set(frequency); }
	void SetOffset1Left(double offset1Left) { this->offset1Left.Set(offset1Left); }
	void SetOffset1Right(double offset1Right) { this->offset1Right.Set(offset1Right); }
	void SetOffset2Left(double offset2Left) { this->offset2Left.Set(offset2Left); }
	void SetOffset2Right(double offset2Right) { this->offset2Right.Set(offset2Right); }
public:
	bool TryGetAmplitudeYScan(double* amplitudeyscan) override { return this->amplitudeYScan.TryGet(amplitudeyscan); }
	bool TryGetFrequency(double* frequency) override { return this->frequency.TryGet(frequency); }
	bool TryGetOffset1Left(double* offset1Left) override { return this->offset1Left.TryGet(offset1Left); }
	bool TryGetOffset1Right(double* offset1Right) override { return this->offset1Right.TryGet(offset1Right); }
	bool TryGetOffset2Left(double* offset2Left) override { return this->offset2Left.TryGet(offset2Left); }
	bool TryGetOffset2Right(double* offset2Right) override { return this->offset2Right.TryGet(offset2Right); }
private:
	Nullable<double> amplitudeYScan;
	Nullable<double> frequency;
	Nullable<double> offset1Left;
	Nullable<double> offset1Right;
	Nullable<double> offset2Left;
	Nullable<double> offset2Right;
};

class CDimensionChannelSPIMDetectionSettings :public libCZI::IDimensionChannelSPIMDetectionSettings
{
public:
	CDimensionChannelSPIMDetectionSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetNADetection(double naDetection) { this->naDetection.Set(naDetection); }
public:
	bool TryGetNADetection(double* naDetection) override { return this->naDetection.TryGet(naDetection); }
private:
	Nullable<double> naDetection;
};

class CDimensionChannelSIMSettings :public libCZI::IDimensionChannelSIMSettings
{
public:
	CDimensionChannelSIMSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetSIMDeviceRef(const std::wstring& simDeviceRef) { this->simDeviceRef.Set(simDeviceRef); }
	void SetGratingPeriod(double gratingPeriod) { this->gratingPeriod.Set(gratingPeriod); }
	void SetGratingRotation(double gratingRotation) { this->gratingRotation.Set(gratingRotation); }
	void SetGridDescription(const std::wstring& gridDescription) { this->gridDescription.Set(gridDescription); }
	void SetReflector(const std::wstring& reflector) { this->reflector.Set(reflector); }
	void SetFilterSetRef(const std::wstring& filterSetRef) { this->filterSetRef.Set(filterSetRef); }
	void SetDiffractionEfficiency1(double diffractionEfficiency1) { this->diffractionEfficiency1.Set(diffractionEfficiency1); }
	void SetDiffractionEfficiency2(double diffractionEfficiency2) { this->diffractionEfficiency2.Set(diffractionEfficiency2); }
	void SetDiffractionEfficiency3(double diffractionEfficiency3) { this->diffractionEfficiency3.Set(diffractionEfficiency3); }
public:
	bool TryGetSIMDeviceRef(std::wstring* simDeviceRef) override { return this->simDeviceRef.TryGet(simDeviceRef); }
	bool TryGetGratingPeriod(double* gratingPeriod) override { return this->gratingPeriod.TryGet(gratingPeriod); }
	bool TryGetGratingRotation(double* gratingRotation) override { return this->gratingRotation.TryGet(gratingRotation); }
	bool TryGetGridDescription(std::wstring* gridDescription) override { return this->gridDescription.TryGet(gridDescription); }
	bool TryGetReflector(std::wstring* reflector) override { return this->reflector.TryGet(reflector); }
	bool TryGetFilterSetRef(std::wstring* filterSetRef) override { return this->filterSetRef.TryGet(filterSetRef); }
	bool TryGetDiffractionEfficiency1(double* diffractionEfficiency1) override { return this->diffractionEfficiency1.TryGet(diffractionEfficiency1); }
	bool TryGetDiffractionEfficiency2(double* diffractionEfficiency2) override { return this->diffractionEfficiency2.TryGet(diffractionEfficiency2); }
	bool TryGetDiffractionEfficiency3(double* diffractionEfficiency3) override { return this->diffractionEfficiency3.TryGet(diffractionEfficiency3); }
private:
	Nullable<std::wstring> simDeviceRef;
	Nullable<double> gratingPeriod;
	Nullable<double> gratingRotation;
	Nullable<std::wstring> gridDescription;
	Nullable<std::wstring> reflector;
	Nullable<std::wstring> filterSetRef;
	Nullable<double> diffractionEfficiency1;
	Nullable<double> diffractionEfficiency2;
	Nullable<double> diffractionEfficiency3;
};

class CDimensionChannelAiryscanSettings : public libCZI::IDimensionChannelAiryscanSettings
{
public:
	CDimensionChannelAiryscanSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetMode(libCZI::IDimensionChannelAiryscanSettings::Mode mode) { this->mode.Set(mode); }
	void SetVirtualPinholeSize(double virtualPinholeSize) { this->virtualPinholeSize.Set(virtualPinholeSize); }
	void SetMagnification(double magnification) { this->magnification.Set(magnification); }
	void SetTransformationXX(double transformationXX) { this->transformationXX.Set(transformationXX); }
	void SetTransformationXY(double transformationXY) { this->transformationXY.Set(transformationXY); }
	void SetTransformationYX(double transformationYX) { this->transformationYX.Set(transformationYX); }
	void SetTransformationYY(double transformationYY) { this->transformationYY.Set(transformationYY); }
public:
	bool TryGetMode(Mode* mode) override { return this->mode.TryGet(mode); }
	std::shared_ptr<libCZI::IFastAiryScanSettings> GetFastAiryScanSettings() override { return this->fastAiryScanSettings; }
	bool TryGetVirtualPinholeSize(double* virtualPinholeSize) override { return this->virtualPinholeSize.TryGet(virtualPinholeSize); }
	bool TryGetMagnification(double* magnification) override { return this->magnification.TryGet(magnification); }
	bool TryGetTransformationXX(double* transformationXX) override { return this->transformationXX.TryGet(transformationXX); }
	bool TryGetTransformationXY(double* transformationXY) override { return this->transformationXY.TryGet(transformationXY); }
	bool TryGetTransformationYX(double* transformationYX) override { return this->transformationYX.TryGet(transformationYX); }
	bool TryGetTransformationYY(double* transformationYY) override { return this->transformationYY.TryGet(transformationYY); }
private:
	Nullable<libCZI::IDimensionChannelAiryscanSettings::Mode> mode;
	Nullable<double> virtualPinholeSize;
	Nullable<double> magnification;
	Nullable<double> transformationXX;
	Nullable<double> transformationXY;
	Nullable<double> transformationYX;
	Nullable<double> transformationYY;
	std::shared_ptr<libCZI::IFastAiryScanSettings> fastAiryScanSettings;
};

class CFastAiryScanSettings : public libCZI::IFastAiryScanSettings
{
public:
	CFastAiryScanSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetIlluminationLinePixels(const libCZI::IFastAiryScanSettings::Vector& illuminationLinePixels) { this->illuminationLinePixels.Set(illuminationLinePixels); }
	void SetIlluminationLine(const libCZI::IFastAiryScanSettings::Vector& illuminationLine) { this->illuminationLine.Set(illuminationLine); }
	void SetFiberMapping(const std::vector<int>& mapping) { this->fiberMapping.Set(mapping); }
	void SetEffectiveNAinX(double effectiveNAinX) { this->effectiveNA_in_x.Set(effectiveNAinX); }
	void SetEffectiveNAinY(double effectiveNAinY) { this->effectiveNA_in_y.Set(effectiveNAinY); }
public:
	bool TryGetIlluminationLinePixels(libCZI::IFastAiryScanSettings::Vector* illuminationLinePixels) override { return this->illuminationLinePixels.TryGet(illuminationLinePixels); }
	bool TryGetIlluminationLine(libCZI::IFastAiryScanSettings::Vector* illuminationLine) override { return this->illuminationLine.TryGet(illuminationLine); }
	bool TryGetFiberMapping(std::vector<int>* mapping) override { return this->fiberMapping.TryGet(mapping); }
	bool TryGetEffectiveNAinX(double* effectiveNAinX) override { return this->effectiveNA_in_x.TryGet(effectiveNAinX); }
	bool TryGetEffectiveNAinY(double* effectiveNAinY) override { return this->effectiveNA_in_y.TryGet(effectiveNAinY); }
private:
	Nullable<libCZI::IFastAiryScanSettings::Vector> illuminationLinePixels;
	Nullable<libCZI::IFastAiryScanSettings::Vector> illuminationLine;
	Nullable<std::vector<int>> fiberMapping;
	Nullable<double> effectiveNA_in_x;
	Nullable<double> effectiveNA_in_y;
};

class CDimensionChannelPolarizingSettings : public libCZI::IDimensionChannelPolarizingSettings
{
public:
	CDimensionChannelPolarizingSettings(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
public:
	void SetPolarizerAngle(double polarizerAngle) { this->polarizerAngle.Set(polarizerAngle); }
	void SetAnalyzerAngle(double analyzerAngle) { this->analyzerAngle.Set(analyzerAngle); }
public:
	bool TryGetPolarizerAngle(double* polarizerAngle) override { return this->polarizerAngle.TryGet(polarizerAngle); }
	bool TryGetAnalyzerAngle(double* analyzerAngle) override { return this->analyzerAngle.TryGet(analyzerAngle); }
private:
	Nullable<double> polarizerAngle;
	Nullable<double> analyzerAngle;
};

class CDimensionChannelRatio :public libCZI::IDimensionChannelRatio
{
public:
	CDimensionChannelRatio(pugi::xml_node node) { this->ParseInformation(node); }
private:
	void ParseInformation(pugi::xml_node node);
	static std::vector<std::wstring> ParseChannelRefSequence(pugi::xml_node node);
public:
	void SetType(Type type)  { this->type.Set(type); }
	void SetConst1(double c1) { this->c1.Set(c1); }
	void SetConst2(double c2) { this->c2.Set(c2); }
	void SetConst3(double c3) { this->c3.Set(c3); }
	void SetConst4(double c4) { this->c4.Set(c4); }
	void SetConst5(double c5) { this->c5.Set(c5); }
	void SetConst6(double c6) { this->c6.Set(c6); }
	void SetSource1(const std::vector<std::wstring>& source1) { this->source1.Set(source1); }
	void SetSource2(const std::vector<std::wstring>& source2) { this->source2.Set(source2); }
public:
	bool TryGetType(Type* type) override { return this->type.TryGet(type); }
	bool TryGetConst1(double* c1) override { return this->c1.TryGet(c1); }
	bool TryGetConst2(double* c2) override { return this->c2.TryGet(c2); }
	bool TryGetConst3(double* c3) override { return this->c3.TryGet(c3); }
	bool TryGetConst4(double* c4) override { return this->c4.TryGet(c4); }
	bool TryGetConst5(double* c5) override { return this->c5.TryGet(c5); }
	bool TryGetConst6(double* c6) override { return this->c6.TryGet(c6); }
	bool TryGetSource1(std::vector<std::wstring>* source1) override { return this->source1.TryGet(source1); }
	bool TryGetSource2(std::vector<std::wstring>* source2) override { return this->source2.TryGet(source2); }
public:
	Nullable<libCZI::IDimensionChannelRatio::Type> type;
	Nullable<double> c1, c2, c3, c4, c5, c6;
	Nullable<std::vector<std::wstring>> source1, source2;
};

struct CXmlEnums
{
	static bool StringToDimensionChannelChannelType(const wchar_t* sz, libCZI::DimensionChannelChannelType* channeltype);
	static bool StringToDimensionChannelAcquisitionMode(const wchar_t* sz, libCZI::DimensionChannelAcquisitionMode* mode);
	static bool StringToDimensionChannelIlluminationType(const wchar_t* sz, libCZI::DimensionChannelIlluminationType* illuminationtype);
	static bool StringToDimensionChannelContrastMethod(const wchar_t* sz, libCZI::DimensionChannelContrastMethod* contrastmethod);
	static bool StringToDimensionChannelPinholeGeometry(const wchar_t* sz, libCZI::DimensionChannelPinholeGeometry* pinholegeometry);
	static bool StringToPixelType(const wchar_t* sz, libCZI::PixelType* pixeltype);
	static bool StringToBinning(const wchar_t* sz, libCZI::ChannelDetectorSettingsBinning* binning);
	static bool StringToShutter(const wchar_t* sz, libCZI::ChannelDetectorSettingsShutter* shutter);
	static bool StringToCameraProcessingMode(const wchar_t* sz, libCZI::ChannelDetectorCameraProcessingMode* mode);
	static bool StringToDimensionChannelLaserScanScanningMode(const wchar_t* sz, libCZI::DimensionChannelLaserScanScanningMode* scanningMode);
	static bool StringToDimensionChannelLaserScanScanDirection(const wchar_t* sz, libCZI::DimensionChannelLaserScanScanDirection* scanDirection);
	static bool StringToDimensionChannelLaserScanAveragingMethod(const wchar_t* sz, libCZI::DimensionChannelLaserScanAveragingMethod* averagingMethod);
	static bool StringToDimensionChannelLaserScanAveragingMode(const wchar_t* sz, libCZI::DimensionChannelLaserScanAveragingMode* averagingMode);
	static bool StringToDimensionChannelLaserScanLaserAttenuatorState(const wchar_t* sz, libCZI::DimensionChannelLaserScanLaserAttenuatorState* attenuatorState);
	static bool StringToDimensionChannelSPIMIlluminationSettingsBeamType(const wchar_t* sz, libCZI::DimensionChannelSPIMIlluminationSettingsBeamType* beamType);
	static bool StringToIInstrumentObjectiveSettingsMedium(const wchar_t* sz, libCZI::IInstrumentObjectiveSettings::Medium* medium);
	static bool StringToIDimensionChannelAiryscanSettingsMode(const wchar_t* sz, libCZI::IDimensionChannelAiryscanSettings::Mode* mode);
	static bool StringToIDimensionChannelRatioType(const wchar_t* sz, libCZI::IDimensionChannelRatio::Type* type);

	static std::vector<double> ParseDoubleXmlArray(const std::wstring& str);
	static std::vector<int> ParseInt32XmlArray(const std::wstring& str);
};