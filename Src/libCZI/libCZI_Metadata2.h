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

#include <cstdint>
#include <string>

#include "libCZI.h"

namespace libCZI
{
    /// Values that represent the type of a channel.
    /// This field (in a sense) has the highest precedence when determining/assigning
    ///	the microscopy method how a channel was acquired. Usually, the AcquisitionMode-field
    ///	is used to give an indication which microscopy method was used. But if this field
    ///	"ChannelType" is present, it has higher precedence - i.e. it should be checked first.
    enum class DimensionChannelChannelType : std::uint8_t
    {
        Heightmap,								///< This channel represents a heightmap. Note that additional restrictions apply when declaring a channel as heightmap.
        PalHR,									///< Unknown denotation.
        PalWidefield,							///< Unknown denotation.
        SimHR,									///< Unknown denotation.
        SimWidefield,							///< Unknown denotation.
        SimDWF,									///< Unknown denotation.
        AiryScanSum,							///< This channel contains a processed (combined) super-resolution from an Airy-scan-acquisition in draft quality.
        AiryScanRawSr,							///< This channel contains the images of all sensors in the Airy-Scan-sensor-array.
        AiryScanRaw,							///< This channel contains the images of all sensors in the Airy-Scan-sensor-array.
        AiryScanSr,								///< This channel contains the (processed) super-resolution image.
        AiryScanVp,								///< This channel contains the (processed) "virtual pinhole" image.
        AiryScanMb,								///< This channel contains the (processed) "multi-beam" image.
        AiryScanRingSheppardSum,				///< This channel contains the Sheppard-sum created from 4 fiber-rings.
        OnlineUnmixing,							///< This channel contains the result of an online unmixing acquisition with a laser-scanning microscope.
        Unspecified								///< Unspecified type.
    };

    /// Values that represent the acquisition mode of a channel.
    /// The acquisition mode describes the type of microscopy performed for each channel.
    enum class DimensionChannelAcquisitionMode : std::uint8_t
    {
        WideField,							///< The channel contains images from wide field microscopy.
        LaserScanningConfocalMicroscopy,	///< The channel contains images from laser-scanning-confocal microscopy.
        SpinningDiskConfocal,				///< The channel contains images from a confocal spinning-disk system.
        SlitScanConfocal,					///< Unknown denotation.
        MultiPhotonMicroscopy,				///< The channel contains images an image from a multi-photon-microscopy system.
        StructuredIllumination,				///< The channel contains images from structured-illumination microscopy system.
        SingleMoleculeImaging,				///< meaning unknown
        TotalInternalReflection,			///< The channel contains images from a TIRF microscopy system.
        FluorescenceLifetime,				///< Unknown denotation.
        SpectralImaging,					///< Unknown denotation.
        FluorescenceCorrelationSpectroscopy,///< Unknown denotation.
        NearFieldScanningOpticalMicroscopy, ///< Unknown denotation.
        SecondHarmonicGenerationImaging,	///< The channel contains images from Second harmonic imaging microscopy (SHIM).
        PALM,								///< The channel contains images from photo-activation localization microscopy.
        STORM,								///< The channel contains images from stochastic optical reconstruction microscopy.
        STED,								///< The channel contains images from stimulated emission depletion microscopy.
        TIRF,								///< The channel contains images from total internal reflection fluorescence (TIRFM).
        FSM,								///< The channel contains images from Fluorescence speckle microscopy.
        LCM,								///< The channel contains images from laser capture micro dissection.
        SPIM,								///< The channel contains images from selective-plane-illumination microscopy.
        SEM,								///< The channel contains images from scanning electron microscopy.
        FIB,								///< The channel contains images from focus ion beam microscopy.
        FIB_SEM,							///< The channel contains images from focus ion beam and scanning electron microscopy.
        ApertureCorrelation,				///< The channel contains image from aperture correlation (a specific application of Structured Illumination).
        Other								///< Some other type of image formation.
    };

    /// Values that represent the method of illumination used to acquire the channel.
    enum class DimensionChannelIlluminationType : std::uint8_t
    {
        Transmitted,						///< Transmitted light was used.
        Epifluorescence,					///< Objective was used to bring light to the specimen.
        Oblique,							///< Oblique light was used.
        NonLinear,							///< Unknown denotation.
        Other								///< Some other type of illumination was used.
    };

    /// Values that represent a contrast method, a technique used to achieve contrast, for the images in a channel.
    enum class DimensionChannelContrastMethod : std::uint8_t
    {
        Brightfield,				///< Bright filed micrsocopy was used.
        Phase,						///< Unknown denotation.
        DIC,						///< Differential Interference Contrast was used.
        HoffmanModulation,			///< Hoffman Modulation Contrast (HMC) was used.
        ObliqueIllumination,		///< Oblique illumination was used.
        PolarizedLight,				///< Polarization microscopy was used.
        Darkfield,					///< Dark Field Imaging was used.
        Fluorescence,				///< Fluorescence microscopy was used.
        MultiPhotonFluorescence,	///< Multi-photon excitation was employed.
        Other						///< Some other contrast method was used.
    };

    /// Values that represent the pinhole geometry.
    enum class DimensionChannelPinholeGeometry : std::uint8_t
    {
        Circular,		///< An enum constant representing a circular pinhole.
        Rectangular,	///< An enum constant representing a rectangular pinhole.
        Other			///< An enum constant representing "some other geometry".
    };

    /// This struct represents either a single value or a range.
    template <typename t>
    struct RangeOrSingleValue
    {
        /// True if this struct represents a single value. In this case, the property end is invalid.
        bool singleValue;

        /// The start value of a range (in the case that singleValue is false) or the single value (if singleValue is true).
        t startOrSingleValue;

        /// The end of the range (only valid if singleValue is false).
        t end;
    };

    /// Structure used to describe the spectral characteristic of light.
    struct SpectrumCharacteristics
    {
        /// Values that represent the "type of information given".
        enum class InformationType
        {
            SinglePeak, ///< An enum constant representing that a single peak is characterized.
            Ranges		///< An enum constant representing that a set of ranges are used to characterize the spectrum.
        };

        /// The type of information given.
        InformationType type;

        /// The single peak (only valid if "type" is "InformationType::SinglePeak").
        double singlePeak;

        /// The set of ranges (only valid if "type" is "InformationType::Ranges").
        std::vector<RangeOrSingleValue<double>> ranges;
    };

    /// This interface is used for all elements that contain an 'id' (as XML-attribute).
    class IAttributeId
    {
    public:
        /// Attempts to get the 'id'. If the method succeeds, the id is put into the specified string.
        /// \param [out] id If non-null, the id is put here (if successful).
        /// \returns True if it succeeds; false otherwise.
        virtual bool TryGetAttributeId(std::wstring* id) = 0;
        virtual ~IAttributeId() = default;
    };

    /// This interface is used for all elements that contain a 'name' (as XML-attribute).
    class IAttributeName
    {
    public:
        /// Attempts to get the 'name'. If the method succeeds, the name is put into the specified string.
        /// \param [out] name If non-null, the name is put here (if successful).
        /// \returns True if it succeeds; false otherwise.
        virtual bool TryGetAttributeName(std::wstring* name) = 0;
        virtual ~IAttributeName() = default;
    };

    class IDimensionChannelDetectorSettings;
    class IDimensionChannelLightSourcesSettings;
    class IDimensionChannelLightPath;
    class IDimensionChannelLaserScanInfo;
    class IDimensionChannelSPIMIlluminationSettings;
    class IDimensionChannelSPIMDetectionSettings;
    class IDimensionChannelSIMSettings;
    class IDimensionChannelPolarizingSettings;
    class IDimensionChannelAiryscanSettings;
    class IDimensionChannelRatio;

    /// Information about the dimension "channel".
    class IDimensionChannelInfo : public IAttributeId, public IAttributeName
    {
    public:
        /// Try to get the channel type.
        /// \param [in,out] type If non-null and successful, the channel type is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetChannelType(DimensionChannelChannelType* type) = 0;

        /// Try to get the channel unit.
        /// \param [in,out] unit If non-null and successful, the channel unit is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetChannelUnit(std::wstring* unit) = 0;

        /// Try to get the pixel type
        /// \param [in,out] pixelType If non-null and successful, the pixel type is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetPixelType(PixelType* pixelType) = 0;

        /// Try to get the component bit count.
        /// \param [in,out] bitCount If non-null and successful, the component bit count is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetComponentBitCount(int* bitCount) = 0;

        /// Try to get the acquisition mode.
        /// \param [in,out] acquisitionMode If non-null and successful, the acquisition mode is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetAcquisitionMode(DimensionChannelAcquisitionMode* acquisitionMode) = 0;

        /// Try to get the illumination type
        /// \param [in,out] illuminationType If non-null and successful, the illumination type is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetIlluminationType(DimensionChannelIlluminationType* illuminationType) = 0;

        /// Try to get the contrast method
        /// \param [in,out] contrastMethod If non-null and successful, the contrast method is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetContrastMethod(DimensionChannelContrastMethod* contrastMethod) = 0;

        /// Try to get the illumination wavelength information.
        /// \param [in,out] illuminationwavelength If non-null and successful, the illumination wavelength information is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetIlluminationWavelength(SpectrumCharacteristics* illuminationwavelength) = 0;

        /// Try to get the detection wavelength information.
        /// \param [in,out] detectionwavelength If non-null and successful, the detection wavelength information is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetDetectionWavelength(SpectrumCharacteristics* detectionwavelength) = 0;

        /// Try to get excitation wavelength.
        /// \param [in,out] excitationwavelength If non-null and successful, the excitation wavelength is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetExcitationWavelength(double* excitationwavelength) = 0;

        /// Try to get emission wavelength.
        /// \param [in,out] emissionwavelength If non-null and successful, the emission wavelength is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetEmissionWavelength(double* emissionwavelength) = 0;

        /// Try to get the effective numerical aperture.
        /// \param [in,out] na If non-null and successful, the effective numerical aperture is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetEffectiveNA(double* na) = 0;

        /// Try to get the dye identifier.
        /// \param [in,out] dyeid If non-null and successful, the dye identifier is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetDyeId(std::wstring* dyeid) = 0;

        /// Try to get the dye database identifier.
        /// \param [in,out] dyedatabaseid If non-null and successful, the dye database identifier is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetDyeDatabaseId(std::wstring* dyedatabaseid) = 0;

        /// Try to get the pinhole size.
        /// \param [in,out] pinholesize If non-null and successful, the pinhole size is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetPinholeSize(double* pinholesize) = 0;

        /// Try to get the pinhole size in units of airy disk size.
        /// \param [in,out] pinholesizeairy If non-null, the pinhole size in units of airy disk size if placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetPinholeSizeAiry(double* pinholesizeairy) = 0;

        /// Try to get the pinhole geometry.
        /// \param [in,out] pinholegeometry If non-null and successful, the pinhole geometry is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetPinholeGeometry(DimensionChannelPinholeGeometry* pinholegeometry) = 0;

        /// Try to get the name of the fluorophor used to produce this channel [plain text string].
        /// \param [in,out] fluor If non-null and successful, the name of the fluorophor is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetFluor(std::wstring* fluor) = 0;

        /// Try to get optical density of ND filters used.
        /// \param [in,out] ndfilter If non-null and successful, the optical density of ND filters used is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetNDFilter(double* ndfilter) = 0;

        /// Try to get the pocket cell setting.
        /// \param [in,out] pocketcellsetting If non-null and successful, the pocket cell setting is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetPocketCellSetting(int* pocketcellsetting) = 0;

        /// Try to get the color.
        /// \param [in,out] color If non-null and successful, the color is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetColor(libCZI::Rgb8Color* color) = 0;

        /// Try to get the exposure time
        /// \param [in,out] exposuretime If non-null and successful, the exposure time is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetExposureTime(RangeOrSingleValue<std::uint64_t>* exposuretime) = 0;

        /// Try to get the depth of focus.
        /// \param [in,out] depthoffocus If non-null and successful, the depth of focus is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetDepthOfFocus(double* depthoffocus) = 0;

        /// Try to get the section thickness.
        /// \param [in,out] sectionthickness If non-null and successful, the section thickness is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetSectionThickness(double* sectionthickness) = 0;

        /// Gets the detector settings.
        /// \returns The detector settings.
        virtual std::shared_ptr<IDimensionChannelDetectorSettings> GetDetectorSettings() = 0;

        /// Gets the light sources settings.
        /// \returns The light sources settings.
        virtual std::shared_ptr<IDimensionChannelLightSourcesSettings> GetLightSourcesSettings() = 0;

        /// Gets the light path.
        /// \returns The light path.
        virtual std::shared_ptr<IDimensionChannelLightPath> GetLightPath() = 0;

        /// Gets the laser scan information.
        /// \returns The laser scan information.
        virtual std::shared_ptr<IDimensionChannelLaserScanInfo> GetLaserScanInfo() = 0;

        /// Gets the SPIM illumination settings
        /// \returns The SPIM illumination settings.
        virtual std::shared_ptr<IDimensionChannelSPIMIlluminationSettings> GetSPIMIlluminationSettings() = 0;

        /// Gets the SPIM detection settings
        /// \returns The SPIM detection settings.
        virtual std::shared_ptr<IDimensionChannelSPIMDetectionSettings> GetSPIMDetectionSettings() = 0;

        /// Gets the SIM settings.
        /// \returns The SIM settings.
        virtual std::shared_ptr<IDimensionChannelSIMSettings> GetSIMSettings() = 0;

        /// Gets the polarizing settings.
        /// \returns The polarizing settings.
        virtual std::shared_ptr<IDimensionChannelPolarizingSettings> GetPolarizingSettings() = 0;

        /// Gets the airy scan settings.
        /// \returns The airy scan settings.
        virtual std::shared_ptr<IDimensionChannelAiryscanSettings> GetAiryscanSettings() = 0;

        /// Try to get the reflector.
        /// \param [in,out] reflector If non-null and successful, the reflector is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetReflector(std::wstring* reflector) = 0;

        /// Try to get the condenser contrast.
        /// \param [in,out] condensorContrast If non-null and successful, the condensor contrast is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetCondenserContrast(std::wstring* condensorContrast) = 0;

        /// Try to get numerical aperture of the condenser.
        /// \param [in,out] naCondensor If non-null, the numerical aperture of the condenser.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetNACondenser(double* naCondensor) = 0;

        /// Gets the ratio (between two active channels).
        /// \returns The ratio.
        virtual std::shared_ptr<IDimensionChannelRatio> GetRatio() = 0;

        virtual ~IDimensionChannelInfo() = default;
    };

    /// Information about the set of channels.
    class IDimensionsChannelsInfo
    {
    public:
        /// Gets the channel count.
        /// \returns The channel count.
        virtual int GetChannelCount() const = 0;

        /// Gets the specified channel.
        /// \param  i Zero-based index of the channel.
        /// \returns The channel object is successful; otherwise a null pointer.
        virtual std::shared_ptr<IDimensionChannelInfo> GetChannel(int i) const = 0;

        virtual ~IDimensionsChannelsInfo() {}
    };

    /// Values that represent the binning mode of a detector.
    enum class ChannelDetectorSettingsBinning : std::uint8_t
    {
        _1x1,	///< 1x1 pixels are used (i.e. no binning).
        _2x2,	///< 2x2 pixels are used.
        _4x4,	///< 4x4 pixels are used.
        _8x8,	///< 8x8 pixels are used.
        Other	///< Some other binning was used.
    };

    /// Values that represent the behavior of the shutter.
    enum class ChannelDetectorSettingsShutter : std::uint8_t
    {
        Rolling,	///< A rolling shutter mode was used.
        Global,		///< A global shutter mode was used.
        Other		///< Some other shutter mode was used.
    };

    /// Values that represent the detector processing mode.
    enum class ChannelDetectorCameraProcessingMode :std::uint8_t
    {
        None,		///< No camera processing was used.
        Widefield,  ///< Widefield processing mode was used.
        Sectioned,	///< Sectioned processing mode was used.
        RawDataMode ///< Raw data mode was used.
    };

    /// The detector settings.
    class IDimensionChannelDetectorSettings
    {
    public:
        /// Try to get reference to detector
        /// \param [in,out] detectorRef If non-null and successful, the detector reference is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDetectorRef(std::wstring* detectorRef) = 0;

        /// Try to get the binning mode.
        /// \param [in,out] binning If non-null and successful, the binning is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetBinning(ChannelDetectorSettingsBinning* binning) = 0;

        /// Try to get the shutter mode.
        /// \param [in,out] shutter If non-null and successful, the shutter mode is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetShutter(ChannelDetectorSettingsShutter* shutter) = 0;

        /// Try to get the photon conversion factor.
        /// \param [in,out] conversionFactor If non-null and successful, the conversion factor is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPhotonConversionFactor(double* conversionFactor) = 0;

        /// Try to get the gain.
        /// \param [in,out] gain If non-null and successful, the gain is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetGain(double* gain) = 0;

        /// Try to get the digital gain.
        /// \param [in,out] digitalgain If non-null and successful, the digital gain is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDigitalGain(double* digitalgain) = 0;

        /// Try to get the offset.
        /// \param [in,out] offset If non-null and successful, the offset is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetOffset(double* offset) = 0;

        /// Try to get the EM gain.
        /// \param [in,out] emgain If non-null and successful, the EM gain is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetEMGain(double* emgain) = 0;

        /// Try to get the voltage.
        /// \param [in,out] voltage If non-null and successful, the voltage is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetVoltage(double* voltage) = 0;

        /// Try to get the read out rate
        /// \param [in,out] readoutrate If non-null and successful, the read out rate is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetReadOutRate(double* readoutrate) = 0;

        /// Try to get a boolean indicating whether brightness/contrast correction was used.
        /// \param [in,out] usebrightnesscontrastcorrection
        /// If non-null and successful, a boolean indicating whether brightness/contrast correction was used is placed here.
        /// usebrightnesscontrastcorrection.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetUseBrightnessContrastCorrection(bool* usebrightnesscontrastcorrection) = 0;

        /// Try to get the pixel maximum.
        /// \param [in,out]  pixelmaximum If non-null and successful, the pixel maximum is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPixelMaximum(double* pixelmaximum) = 0;

        /// Try to get the pixel accuracy.
        /// \param [in,out] pixelaccuracy If non-null and successful, the pixel accuracy is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPixelAccuracy(double* pixelaccuracy) = 0;

        /// Try to get the pixel scale factor.
        /// \param [in,out] pixelscalefactor If non-null and successful, the pixel scale factor is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPixelScaleFactor(double* pixelscalefactor) = 0;

        /// Try to get the camera processing mode.
        /// \param [in,out] mode If non-null and successful, the camera processing mode is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCameraProcessingMode(ChannelDetectorCameraProcessingMode* mode) = 0;

        virtual ~IDimensionChannelDetectorSettings() {}
    };

    /// The light sources settings.
    class IDimensionChannelLightSourcesSetting
    {
    public:
        /// Description of the polarization.
        struct Polarization
        {
            bool stokesParameterValid[4];   ///< A boolean array, indicating whether the corresponding values are valid.
            double stokesParameter[4];		///< The stokes parameters, for validity of a value check the corresponding boolean.
        };

        /// Try to get the light source reference
        /// \param [in,out] lightsourceref If non-null, the lightsourceref.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLightSourceRef(std::wstring* lightsourceref) = 0;

        /// Try to get the wavelength.
        /// \param [in,out] wavelength If non-null and successful, the wavelength is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetWavelength(double* wavelength) = 0;

        /// Try to get the attenuation.
        /// \param [in,out] attenuation If non-null and successful, the attenuation is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAttenuation(double* attenuation) = 0;

        /// Try to get the transmission.
        /// \param [in,out] transmission If non-null and successful, the transmission is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTransmission(double* transmission) = 0;

        /// Try to get the intensity.
        /// \param [in,out] intensity If non-null and successful, the intensity is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetIntensity(std::wstring* intensity) = 0;

        /// Try to get the flash duration.
        /// \param [in,out] flashduration If non-null and successful, the flash duration is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetFlashDuration(double* flashduration) = 0;

        /// Try to get the TIRF angle.
        /// \param [in,out] tirfangle If non-null and successful, the TIRF angle is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTIRFAngle(double* tirfangle) = 0;

        /// Try to get the TIRF depth of penetration.
        /// \param [in,out] tirfdepthofpenetration If non-null and successful, the TIRF depth of penetration is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTIRFDepthOfPenetration(double* tirfdepthofpenetration) = 0;

        /// Try to get a boolean indicating whether TIRF was calibrated.
        /// \param [in,out] isTIRFcalibrated If non-null and successful, a boolean indicating whether TIRF was calibrated is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetIsTIRFCalibrated(bool* isTIRFcalibrated) = 0;

        /// Try to get a boolean indicating whether a tunable laser line was used.
        /// \param [in,out] isnlo If non-null and successful, a boolean indicating whether a tunable laser line was used is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetIsNlo(bool* isnlo) = 0;

        /// Try to get polarization information.
        /// \param [in,out]  polarization If non-null and successful, the polarization information is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPolarization(Polarization* polarization) = 0;

        virtual ~IDimensionChannelLightSourcesSetting() {};
    };

        /// The light sources settings keyed by channel index.
    class IDimensionChannelLightSourcesSettings
    {
    public:
        /// Gets the count of light source settings.
        /// \returns The light source setting count.
        virtual int GetLightSourceSettingCount() const = 0;

        /// Gets light source setting for the specified channel.
        /// \param  i Zero-based Index of the channel.
        /// \returns  The light source setting object if available; and empty pointer otherwise.
        virtual std::shared_ptr<IDimensionChannelLightSourcesSetting> GetLightSourceSetting(int i) const = 0;

        virtual ~IDimensionChannelLightSourcesSettings() {};
    };

    /// The light path information.
    class IDimensionChannelLightPath
    {
    public:
        /// Try to get reference to the excitation filter.
        /// \param [in,out] excitationFilterRef
        /// If non-null and successful, the reference to the excitation filter is placed here.
        /// reference.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetExcitationFilterRef(std::wstring* excitationFilterRef) = 0;

        /// Try to get reference to the dichroic filter.
        /// \param [in,out] dichroicRef If non-null and successful, the reference to the dichroic filter is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDichroicFilterRef(std::wstring* dichroicRef) = 0;

        /// Try to get reference to the emission filter.
        /// \param [in,out] emissionFilterRef
        /// If non-null and successful, the  reference to the emission filter is placed here.
        /// reference.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetEmissionFilterRef(std::wstring* emissionFilterRef) = 0;

        virtual ~IDimensionChannelLightPath() = default;
    };

    /// Values that represent the laser scanning mode.
    enum class DimensionChannelLaserScanScanningMode : std::uint8_t
    {
        Frame,				///< An enum constant representing the frame scanning mode.
        FastFrame,			///< An enum constant representing the fast frame scanning mode.
        LineSequential,		///< An enum constant representing the line sequential scanning mode.
        Line,				///< An enum constant representing the line scanning mode.
        Spot,				///< An enum constant representing the spot scanning mode.
        Other				///< An enum constant representing some other scanning mode was used.
    };

    /// Values that represent the laser scan direction.
    enum class DimensionChannelLaserScanScanDirection : std::uint8_t
    {
        Bidirectional,  ///< An enum constant representing the bidirectional scan direction.
        Unidirectional  ///< An enum constant representing the unidirectional scan direction.
    };

    /// Values that represent the laser scan averaging methods.
    enum class DimensionChannelLaserScanAveragingMethod : std::uint8_t
    {
        Mean,   ///< An enum constant representing the mean laser scan averaging method.
        Sum,	///< An enum constant representing the sum laser scan averaging method.
        Other	///< An enum constant representing that some other averaging method was used.
    };

    /// Values that represent the laser scan averaging mode.
    enum class DimensionChannelLaserScanAveragingMode : std::uint8_t
    {
        Frame,  ///< An enum constant representing the frame laser scan averaging mode.
        Line,	///< An enum constant representing the line laser scan averaging mode.
        Other	///< An enum constant representing that some other line laser scan averaging mode was used.
    };

    /// Values that represent the channel laser scan laser attenuator state.
    enum class DimensionChannelLaserScanLaserAttenuatorState : std::uint8_t
    {
        On, ///< An enum constant representing the on option.
        Off ///< An enum constant representing the off option.
    };

    /// Information about the laser scan.
    class IDimensionChannelLaserScanInfo
    {
    public:
        /// Try to get the pixel time.
        /// \param [in,out] pixelTime If non-null and successful, the pixel time is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPixelTime(double* pixelTime) = 0;

        /// Try to get the line time.
        /// \param [in,out] lineTime If non-null and successful, the line time is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLineTime(double* lineTime) = 0;

        /// Try to get the frame time.
        /// \param [in,out] frameTime If non-null and successful, the frame time is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetFrameTime(double* frameTime) = 0;

        /// Try to get the averaging (how many times the scanning was repeated).
        /// \param [in,out] averaging If non-null and successful, the averaging is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAveraging(int* averaging) = 0;

        /// Try to get the scanning mode.
        /// \param [in,out] mode If non-null and successful, the mode is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetScanningMode(DimensionChannelLaserScanScanningMode* mode) = 0;

        /// Try to get the sample rotation.
        /// \param [in,out] sampleRotation If non-null and successful, the sample rotation is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetSampleRotation(double* sampleRotation) = 0;

        /// Try to get the sample offset in x-direction.
        /// \param [in,out] sampleOffsetX If non-null and successful, the sample offset in x direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetSampleOffsetX(double* sampleOffsetX) = 0;

        /// Try to get the sample offset in y-direction.
        /// \param [in,out] sampleOffsetY If non-null and successful, the sample offset in y direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetSampleOffsetY(double* sampleOffsetY) = 0;

        /// Try to get the scan direction.
        /// \param [in,out] scanDirection
        /// If non-null and successful, the scan direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetScanDirection(DimensionChannelLaserScanScanDirection* scanDirection) = 0;

        /// Try to get the zoom in x direction.
        /// \param [in,out] zoomX If non-null, the zoom in x direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetZoomX(double* zoomX) = 0;

        /// Try to get the zoom in y direction.
        /// \param [in,out] zoomY If non-null, the zoom in y direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetZoomY(double* zoomY) = 0;

        /// Try to get the zoom in z direction.
        /// \param [in,out] zoomZ If non-null, the zoom in z direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetZoomZ(double* zoomZ) = 0;

        /// Attempts to get the crop offset
        /// \param [in,out] cropOffset If non-null and successful, the crop offset is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCropOffset(double* cropOffset) = 0;

        /// Try to get the attenuation of the laser.
        /// \param [in,out] laserAttenuatorMeas If non-null and successful, the attenuation of the laser is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLaserAttenuatorMeas(double* laserAttenuatorMeas) = 0;

        /// Try to get the laser blanking (time the laser is not active while re-positioning for the next scan line).
        /// \param [in,out] laserBlanking If non-null and successful, the laser blanking time is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLaserBlanking(double* laserBlanking) = 0;

        /// Try to get the attenuation of the laser for bleaching.
        /// \param [in,out] laserAttenuatorBleach If non-null, the attenuator of the laser for bleaching is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLaserAttenuatorBleach(double* laserAttenuatorBleach) = 0;

        /// Try to get the line step.
        /// \param [in,out] lineStep If non-null and successful, the line step is place here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLineStep(std::uint32_t* lineStep) = 0;

        /// Try to get the scan speed.
        /// \param [in,out] scanSpeed If non-null and successful, the scan speed is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetScanSpeed(int* scanSpeed) = 0;

        /// Try to get the averaging method.
        /// \param [in,out] averagingMethod
        /// If non-null and successful, the averaging method is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAveragingMethod(DimensionChannelLaserScanAveragingMethod* averagingMethod) = 0;

        /// Try to get the averaging mode.
        /// \param [in,out] averagingMode If non-null and successful, the averaging mode is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAveragingMode(DimensionChannelLaserScanAveragingMode* averagingMode) = 0;

        /// Try to get the laser attenuator state
        /// \param [in,out] attenuatorState If non-null and successful, the laser attenuator state is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetLaserAttenuatorState(DimensionChannelLaserScanLaserAttenuatorState* attenuatorState) = 0;

        virtual ~IDimensionChannelLaserScanInfo() = default;
    };

    /// Values that represent the beam type for SPIM illumination.
    enum class DimensionChannelSPIMIlluminationSettingsBeamType : std::uint8_t
    {
        Gauss,  ///< An enum constant representing the Gauss beam type.
        Bessel, ///< An enum constant representing the Bessel beam type.
        Other	///< An enum constant representing some other beam type.
    };

    class IInstrumentObjectiveSettings;
    class ISPIMIlluminationResonanceScannerSettings;
    class ISPIMIlluminationGalvoScannerSettings;

    /// The SPIM illumination settings.
    class IDimensionChannelSPIMIlluminationSettings
    {
    public:
        /// Try to get the numerical aperture of the light sheet.
        /// \param [in,out] naLightSheet If non-null and successful, the numerical aperture of the light sheet is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetNALightSheet(double* naLightSheet) = 0;

        /// Try to get the beam type.
        /// \param [in,out] beamType If non-null and successful, the type of the beam is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetBeamType(DimensionChannelSPIMIlluminationSettingsBeamType* beamType) = 0;

        /// Try to get reference to the cylinder lens.
        /// \param [in,out] cylinderLensRef If non-null and successful, the reference to the cylinder lens is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCylinderLensRef(std::wstring* cylinderLensRef) = 0;

        /// Gets the illumination objective settings.
        /// \returns The illumination objective settings if available, otherwise an empty pointer.
        virtual std::shared_ptr<IInstrumentObjectiveSettings> GetIlluminationObjectiveSettings() = 0;

        /// Gets the resonance scanner settings.
        /// \returns The resonance scanner settings if available, otherwise an empty pointer.
        virtual std::shared_ptr<ISPIMIlluminationResonanceScannerSettings> GetResonanceScannerSettings() = 0;

        /// Gets the galvo scanner settings
        /// \returns The galvo scanner settings if available, otherwise an empty pointer.
        virtual std::shared_ptr<ISPIMIlluminationGalvoScannerSettings> GetGalvoScannerSettings() = 0;

        virtual ~IDimensionChannelSPIMIlluminationSettings() = default;
    };

    /// The objective settings.
    class IInstrumentObjectiveSettings
    {
    public:
        /// Values that represent the medium.
        enum class Medium : std::uint8_t
        {
            Air,			///< An enum constant representing the medium air.
            Oil,			///< An enum constant representing the medium oil.
            Water,			///< An enum constant representing the medium water.
            Glycerol,		///< An enum constant representing the medium glycerol.
            Other			///< An enum constant representing that some other medium was used.
        };

        /// Try to get reference to the objective.
        /// \param [in,out] objectiveRef If non-null and successful, the reference to the objective is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetObjectiveRef(std::wstring* objectiveRef) = 0;

        /// Try to get the medium.
        /// \param [in,out] medium If non-null and successful, the medium is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetMedium(Medium* medium) = 0;

        /// Try to get the value of the first correction collar.
        /// \param [in,out] correctioncollar If non-null and successful, the value of the first correction collar is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCorrectionCollar(double* correctioncollar) = 0;

        /// Try to get the value of the second correction collar.
        /// \param [in,out] correctioncollar2 If non-null and successful, the value of the second correction collar is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCorrectionCollar2(double* correctioncollar2) = 0;

        /// Try to get the mode of the correction controller.
        /// \param [in,out] correctioncontrollermode 
        /// If non-null and successful, the mode of the correction controller is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCorrectionControllerMode(std::wstring* correctioncontrollermode) = 0;

        /// Try to get the bottom thickness of the correction controller.
        /// \param [in,out] correctioncontrollerbottomthickness
        /// If non-null and successful, the
        /// bottom thickness of the correction controller is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCorrectionControllerBottomThickness(double* correctioncontrollerbottomthickness) = 0;

        /// Try to get the imaging depth of the correction controller.
        /// \param [in,out] correctioncontrollerimagingdepth
        /// If non-null and successful, the
        /// imaging depth of the correction controller is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetCorrectionControllerImagingDepth(double* correctioncontrollerimagingdepth) = 0;

        /// Try to get the refractive index of the immersion medium.
        /// \param [in,out] refractiveindex If non-null and successful, the refractive index of the immersion medium is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetRefractiveIndex(double* refractiveindex) = 0;

        virtual ~IInstrumentObjectiveSettings() = default;
    };

    /// The SPIM illumination resonance scanner settings.
    class ISPIMIlluminationResonanceScannerSettings
    {
    public:
        /// Try to get the amplitude.
        /// \param [in,out] amplitude If non-null and successful, the amplitude is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAmplitude(double* amplitude) = 0;

        virtual ~ISPIMIlluminationResonanceScannerSettings() = default;
    };

    /// The settings of the SPIM galvo scanner.
    class ISPIMIlluminationGalvoScannerSettings
    {
    public:
        /// Try to get the amplitude in y direction.
        /// \param [in,out] amplitudeYScan If non-null and successful, the amplitude amplitude in y direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAmplitudeYScan(double* amplitudeYScan) = 0;

        /// Try to get the frequency.
        /// \param [in,out] frequency If non-null and successful, the frequency is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetFrequency(double* frequency) = 0;

        /// Try to get the "offset 1 left".
        /// \param [in,out] offset1Left If non-null and successful, the "offset 1 left" is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetOffset1Left(double* offset1Left) = 0;

        /// Try to get the "offset 1 right".
        /// \param [in,out] offset1Right If non-null and successful, the "offset 1 right" is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetOffset1Right(double* offset1Right) = 0;

        /// Try to get the "offset 2 left".
        /// \param [in,out] offset2Left If non-null and successful, the "offset 2 left" is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetOffset2Left(double* offset2Left) = 0;

        /// Try to get the "offset 2 right".
        /// \param [in,out] offset2Right If non-null and successful, the "offset 2 right" is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetOffset2Right(double* offset2Right) = 0;

        virtual ~ISPIMIlluminationGalvoScannerSettings() = default;
    };

    /// The SPIM detection settings.
    class IDimensionChannelSPIMDetectionSettings
    {
    public:
        /// Try to get the numerical aperture of the detection.
        /// \param [in,out] naDetection If non-null and successful, the numerical aperture of the detection is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetNADetection(double* naDetection) = 0;

        virtual ~IDimensionChannelSPIMDetectionSettings() = default;
    };

    /// The SIM (Structured Illumination Microscopy) settings.
    class IDimensionChannelSIMSettings
    {
    public:
        /// Try to get reference to the SIM device.
        /// \param [in,out] simDeviceRef If non-null and successful, the reference to the SIM device is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetSIMDeviceRef(std::wstring* simDeviceRef) = 0;

        /// Try to get the grating period.
        /// \param [in,out] gratingPeriod If non-null and successful, the grating period is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetGratingPeriod(double* gratingPeriod) = 0;

        /// Try to get the grating rotation-
        /// \param [in,out] gratingRotation If non-null and successful, the grating rotation is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetGratingRotation(double* gratingRotation) = 0;

        /// Try to get the grid description.
        /// \param [in,out] gridDescription If non-null and successful, information describing the grid is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetGridDescription(std::wstring* gridDescription) = 0;

        /// Try to get reference to reflector.
        /// \param [in,out] reflector If non-null and successful, the reference to the reflector is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetReflector(std::wstring* reflector) = 0;

        /// Try to get reference to filter set.
        /// \param [in,out] filterSetRef If non-null and successful, the reference to the filter set is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetFilterSetRef(std::wstring* filterSetRef) = 0;

        /// Try to get the diffraction efficiency for diffraction order -1.
        /// \param [in,out] diffractionEfficiency1 If non-null and successful, the diffraction efficiency for diffraction order -1 is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDiffractionEfficiency1(double* diffractionEfficiency1) = 0;

        /// Try to get the diffraction efficiency for diffraction order 0.
        /// \param [in,out] diffractionEfficiency2 If non-null and successful, the diffraction efficiency for diffraction order 0 is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDiffractionEfficiency2(double* diffractionEfficiency2) = 0;

        /// Try to get the diffraction efficiency for diffraction order 1.
        /// \param [in,out] diffractionEfficiency3 If non-null and successful, the diffraction efficiency for diffraction order 1 is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetDiffractionEfficiency3(double* diffractionEfficiency3) = 0;

        virtual ~IDimensionChannelSIMSettings() = default;
    };

    /// The polarizer settings.
    class IDimensionChannelPolarizingSettings
    {
    public:
        /// Try to get the polarizer angle.
        /// \param [in,out] polarizerAngle If non-null, the polarizer angle.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetPolarizerAngle(double* polarizerAngle) = 0;

        /// Attempts to get analyzer angle
        /// \param [in,out] analyzerAngle If non-null, the analyzer angle.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetAnalyzerAngle(double* analyzerAngle) = 0;

        virtual ~IDimensionChannelPolarizingSettings() = default;
    };

    /// The fast airy scan settings.
    class IFastAiryScanSettings
    {
    public:
        /// A 2D-vector.
        struct Vector
        {
            double x;   ///< The x coordinate.
            double y;	///< The y coordinate.
        };

        /// Try to get the illumination line in pixels.
        /// \param [in,out] illuminationLinePixels If non-null and successful, the illumination line in pixels is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetIlluminationLinePixels(Vector* illuminationLinePixels) = 0;

        /// Try to get the illumination line.
        /// \param [in,out] illuminationLine If non-null and successful, the illumination line is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetIlluminationLine(Vector* illuminationLine) = 0;

        /// Try to get the fiber mapping.
        /// \param [in,out] mapping If non-null and successful, the fiber mapping is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetFiberMapping(std::vector<int>* mapping) = 0;

        /// Try to get the effective numerical aperture in x direction.
        /// \param [in,out] effectiveNAinX If non-null and successful, the effective numerical aperture in x direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetEffectiveNAinX(double* effectiveNAinX) = 0;

        /// Try to get the effective numerical aperture in y direction.
        /// \param [in,out] effectiveNAinY If non-null and successful, the effective numerical aperture in y direction is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetEffectiveNAinY(double* effectiveNAinY) = 0;

        virtual ~IFastAiryScanSettings() = default;
    };

    /// The airy scan settings.
    class IDimensionChannelAiryscanSettings
    {
    public:
        /// Values that represent the mode.
        enum class Mode :std::uint8_t
        {
            SuperResolution,    ///< An enum constant representing the super resolution option.
            VirtualPinhole,		///< An enum constant representing the virtual pinhole option.
            MultiBeam,			///< An enum constant representing the multi beam option.
            Off					///< An enum constant representing the off option.
        };

        /// Try to get the mode.
        /// \param [in,out] mode If non-null and successful, the mode is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetMode(Mode* mode) = 0;

        /// Gets fast airy scan settings.
        /// \returns The fast airy scan settings object if successful, and empty pointer otherwise.
        virtual std::shared_ptr<IFastAiryScanSettings> GetFastAiryScanSettings() = 0;

        /// Try to get the virtual pinhole size.
        /// \param [in,out] virtualPinholeSize If non-null and successful, the size of the virtual pinhole is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetVirtualPinholeSize(double* virtualPinholeSize) = 0;

        /// Try to get the magnification.
        /// \param [in,out] magnification If non-null and successful, the magnification is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetMagnification(double* magnification) = 0;

        /// Try to get the transformation for XX.
        /// \param [in,out] transformationXX If non-null and successful, the transformation for XX is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTransformationXX(double* transformationXX) = 0;

        /// Try to get the transformation for XY.
        /// \param [in,out] transformationXY If non-null and successful, the transformation for XY is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTransformationXY(double* transformationXY) = 0;

        /// Try to get the transformation for YX.
        /// \param [in,out] transformationYX If non-null and successful, the transformation for YX is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTransformationYX(double* transformationYX) = 0;

        /// Try to get the transformation for YY.
        /// \param [in,out] transformationYY If non-null and successful, the transformation for YY is placed here.
        /// \returns {bool} True if it succeeds, false if it fails.
        virtual bool TryGetTransformationYY(double* transformationYY) = 0;

        virtual ~IDimensionChannelAiryscanSettings() = default;
    };

    /// Information about the channel ratio.
    class IDimensionChannelRatio
    {
    public:
        /// The type of an online calculation where the channel is the destination.
        ///
        ///		The following values are possible :
        ///
        ///		NoOnlineCalculation - The data channel receives raw scan data.
        ///			OnlineRatio - (S1 + C1) / (S2 + C2) * C3 + C4
        ///			OnlineSubtraction - (S1 * C1 - S2 * C3) / C2 + C4
        ///			OnlineSubtractionRatio - (S1 - S2 * C1) / (S1 + S2 * C2) * C3 + C4
        ///			OnlineHillFunction - C3 * (C4 / C5) * (S1 - C1) / (C2 - S2)
        ///			OnlineReferenceRatio - (S1 - S2 + C1) / (S2 - C2) * C3 + C4
        ///			OnlineLinearUnmixing - The Linear Unmixing processing.
        ///
        ///			The operands and constants are :
        ///
        ///		    S1 - Source1
        ///			S2 - Source2
        ///			C1 - RatioConstant1
        ///			C2 - RatioConstant2
        ///			C3 - RatioConstant3
        ///			C4 - RatioConstant4
        ///			C5 - RatioConstant5
        enum class Type : std::uint8_t
        {
            NoOnlineCalculation,		///< An enum constant representing the "no online calculation option".
            OnlineRatio,				///< An enum constant representing the "online ratio calculation option".
            OnlineSubtraction,			///< An enum constant representing the "online subtraction calculation option".
            OnlineSubtractionRatio,		///< An enum constant representing the "online subtraction and ratio calculation option".
            OnlineHillFunction,			///< An enum constant representing the "online Hill function calculation option".
            OnlineReferenceRatio,		///< An enum constant representing the "online reference ratio calculation option".
            OnlineLinearUnmixing		///< An enum constant representing the "online un-mixing calculation option".
        };

        /// Try to get the type.
        /// \param [in,out] type If non-null and successful, the type is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetType(Type* type) = 0;

        /// Try to get the constant #1.
        /// \param [in,out] c1 If non-null and successful, the constant #1 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst1(double* c1) = 0;

        /// Try to get the constant #2.
        /// \param [in,out] c2 If non-null and successful, the constant #2 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst2(double* c2) = 0;

        /// Try to get the constant #3.
        /// \param [in,out] c3 If non-null and successful, the constant #3 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst3(double* c3) = 0;

        /// Try to get the constant #4.
        /// \param [in,out] c4 If non-null and successful, the constant #4 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst4(double* c4) = 0;

        /// Try to get the constant #5.
        /// \param [in,out] c5 If non-null and successful, the constant #5 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst5(double* c5) = 0;

        /// Try to get the constant #6.
        /// \param [in,out] c6 If non-null and successful, the constant #6 is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetConst6(double* c6) = 0;

        /// Specifies the source operands S1 for an online calculation. The vector contains
        /// references to channels (ChannelRefs).
        /// \param [in,out] source1 If non-null and the property is available, it will be put here.
        /// \returns True if it succeeds, false otherwise.
        virtual bool TryGetSource1(std::vector<std::wstring>* source1) = 0;

        /// Specifies the source operands S2 for an online calculation. The vector contains
        /// references to channels (ChannelRefs).
        /// \param [in,out] source2 If non-null and the property is available, it will be put here.
        /// \returns True if it succeeds, false otherwise.
        virtual bool TryGetSource2(std::vector<std::wstring>* source2) = 0;

        virtual ~IDimensionChannelRatio() = default;
    };
}
