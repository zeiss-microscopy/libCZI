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

#include "ImportExport.h"
#include "libCZI_DimCoordinate.h"
#include "libCZI_Pixels.h"
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cmath>

namespace libCZI
{
	struct SubBlockStatistics;

	/// This structure specifies the information in an XSD-DateTime field (cf. https://www.w3schools.com/XML/schema_dtypes_date.asp).
	struct LIBCZI_API XmlDateTime
	{
		int sec;   ///< Seconds after the minute - [0, 60] including leap second
		int min;   ///< minutes after the hour - [0, 59]
		int hour;  ///< hours since midnight - [0, 23]
		int mday;  ///< day of the month - [1, 31]
		int mon;   ///< months since January - [0, 11]
		int year;  ///< year [-9999 - 9999]

		bool isUTC;///< True if this object is specifying the time-date in UTC.

		/// The hours of the timezone-offset. If greater than 24 or less than -24, it indicates an invalid timezone-offset.
		int offsetHours;

		/// The minutes of the timezone-offset. If greater than 60 or negative, it indicates an invalid timezone-offset.
		int offsetMinutes;

		/// Clears this object to its blank/initial state.
		void Clear()
		{
			this->sec = this->min = this->hour = this->mday = this->mon = this->year = 0;
			this->isUTC = false;
			this->offsetHours = (std::numeric_limits<int>::min)();
			this->offsetMinutes = (std::numeric_limits<int>::min)();
		}

		/// Query if this object uses a "time zone offset". This is the case of the fields offsetHours and offsetMinutes
		/// contain valid values and "isUTC" is false.
		/// \return True if a "time zone offset" is specified and it is used, false if not.
		bool HasTimeZoneOffset() const
		{
			if (this->isUTC)
			{
				return false;
			}

			if (this->offsetHours <= -24 || this->offsetHours >= 24 || this->offsetMinutes >= 60 || this->offsetMinutes < 0)
			{
				return false;
			}

			return true;
		}

		/// Converts the information into a string conforming to the XSD-specification for the DateTime data type.
		/// If this object does not contain valid information, an exception of type illegal_argument is thrown.
		/// \return This object as a string.
		std::string ToXmlString() const;

		/// Converts the information into a string conforming to the XSD-specification for the DateTime data type.
		/// If this object does not contain valid information, an exception of type illegal_argument is thrown.
		/// \return This object as a string.
		std::wstring ToXmlWstring() const;

		/// Query if this object contains valid information.
		/// \return True if valid, false if not.
		bool IsValid() const;

		/// Attempts to parse an XmlDateTime from the given string. The string needs to
		/// conform to ISO8601.
		/// \param 		    sz		    The string to parse.
		/// \param [in,out] ptrDateTime If non-null and the parsing was successful, the information will be put here.
		/// \returns True if it succeeds, false if it fails.
		static bool TryParse(const char* sz, XmlDateTime* ptrDateTime);

		/// Attempts to parse an XmlDateTime from the given string. The string needs to
		/// conform to ISO8601.
		/// \param 		    szw		    The string to parse.
		/// \param [in,out] ptrDateTime If non-null and the parsing was successful, the information will be put here.
		/// \returns True if it succeeds, false if it fails.
		static bool TryParse(const wchar_t* szw, XmlDateTime* ptrDateTime);
	};

	/// General document information - corresponding to Information/Document.
	struct GeneralDocumentInfo
	{
		/// Default constructor - all fields are intially marked "invalid".
		LIBCZI_API GeneralDocumentInfo() : name_valid(false), title_valid(false), userName_valid(false), description_valid(false), comment_valid(false), keywords_valid(false), rating_valid(false), rating(0), creationDateTime_valid(false) {}
		bool name_valid;				///< Whether the field #name is valid.		
		std::wstring name;				///< Name of the document.
		bool title_valid;				///< Whether the field #title is valid.			
		std::wstring title;				///< Title of the document.
		bool userName_valid;			///< Whether the field #userName is valid.			
		std::wstring userName;			///< Name of the user who created the document.
		bool description_valid;			///< Whether the field #description is valid.			
		std::wstring description;		///< A text describing the document.
		bool comment_valid;				///< Whether the field #comment is valid.			
		std::wstring comment;			///< A text with comments on the document.
		bool keywords_valid;			///< Whether the field #keywords is valid.			
		std::wstring keywords;			///< List of keywords (should be separated by semicolons)
		bool rating_valid;				///< Whether the field #rating is valid.			
		int rating;						///< An integer specifying a "five-star-rating" (should be between 0 and 5).
		bool creationDateTime_valid;	///< Whether the field #creationDateTime is valid.			
		std::wstring creationDateTime;	///< The creation date of the document (formatted as xml-datatype "dateTime").

		/// Sets the name and marks it valid if the specified string is non-null and non-empty. Otherwise, the name is set to invalid.
		///
		/// \param sz The string to set as name.
		LIBCZI_API void SetName(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::name, &GeneralDocumentInfo::name_valid, sz); }

		/// Sets the name and marks it valid if the specified string is non-empty. Otherwise, the name is set to invalid.
		///
		/// \param str The string to set as name.
		LIBCZI_API void SetName(const std::wstring& str) { this->SetName(str.c_str()); }

		/// Sets the title and marks it valid if the specified string is non-null and non-empty. Otherwise, the title is set to invalid.
		///
		/// \param sz The string to set as title.
		LIBCZI_API void SetTitle(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::title, &GeneralDocumentInfo::title_valid, sz); }

		/// Sets the title and marks it valid if the specified string is non-empty. Otherwise, the title is set to invalid.
		///
		/// \param str The string to set as title.
		LIBCZI_API void SetTitle(const std::wstring& str) { this->SetTitle(str.c_str()); }

		/// Sets the username and marks it valid if the specified string is non-null and non-empty. Otherwise, the username is set to invalid.
		///
		/// \param sz The string to set as username.
		LIBCZI_API void SetUserName(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::userName, &GeneralDocumentInfo::userName_valid, sz); }

		/// Sets the username and marks it valid if the specified string is non-empty. Otherwise, the username is set to invalid.
		///
		/// \param str The string to set as username.
		LIBCZI_API void SetUserName(const std::wstring& str) { this->SetUserName(str.c_str()); }

		/// Sets the description and marks it valid if the specified string is non-null and non-empty. Otherwise, the description is set to invalid.
		///
		/// \param sz The string to set as description.
		LIBCZI_API void SetDescription(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::description, &GeneralDocumentInfo::description_valid, sz); }

		/// Sets the description and marks it valid if the specified string is non-empty. Otherwise, the description is set to invalid.
		///
		/// \param str The string to set as description.
		LIBCZI_API void SetDescription(const std::wstring& str) { this->SetDescription(str.c_str()); }

		/// Sets the comment and marks it valid if the specified string is non-null and non-empty. Otherwise, the comment is set to invalid.
		///
		/// \param sz The string to set as comment.
		LIBCZI_API void SetComment(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::comment, &GeneralDocumentInfo::comment_valid, sz); }

		/// Sets the comment and marks it valid if the specified string is non-empty. Otherwise, the comment is set to invalid.
		///
		/// \param str The string to set as comment.
		LIBCZI_API void SetComment(const std::wstring& str) { this->SetComment(str.c_str()); }

		/// Sets the keywords and marks it valid if the specified string is non-null and non-empty. Otherwise, the keywords is set to invalid.
		///
		/// \param sz The string to set as keywords.
		LIBCZI_API void SetKeywords(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::keywords, &GeneralDocumentInfo::keywords_valid, sz); }

		/// Sets the keywords and marks it valid if the specified string is non-empty. Otherwise, the keywords is set to invalid.
		///
		/// \param str The string to set as keywords.
		LIBCZI_API void SetKeywords(const std::wstring& str) { this->SetKeywords(str.c_str()); }

		/// Sets the rating and marks it as valid if the specified rating is non-negative. Otherwise, the rating is set to invalid.
		/// \param rating The rating.
		LIBCZI_API void SetRating(int rating) { this->rating = rating; this->rating_valid = rating >= 0 ? true : false; }

		/// Sets the creationDateTime and marks it valid if the specified string is non-null and non-empty. Otherwise, the creationDateTime is set to invalid.
		///
		/// \param sz The string to set as creationDateTime.
		LIBCZI_API void SetCreationDate(const wchar_t* sz) { this->SetMember(&GeneralDocumentInfo::creationDateTime, &GeneralDocumentInfo::creationDateTime_valid, sz); }

		/// Sets the creationDateTime and marks it valid if the specified string is non-empty. Otherwise, the creationDateTime is set to invalid.
		///
		/// \param str The string to set as creationDateTime.
		LIBCZI_API void SetCreationDate(const std::wstring& str) { this->SetCreationDate(str.c_str()); }

		/// Sets the creationDateTime and marks it valid if the specified XmlDateTime-struct pointer is non-null and valid. Otherwise, the creationDateTime is set to invalid.
		///
		/// \param dateTime The XmlDateTime-struct to set as creationDateTime.
		LIBCZI_API void SetCreationDate(const XmlDateTime* dateTime) { if (dateTime != nullptr && dateTime->IsValid()) { this->SetCreationDate(dateTime->ToXmlWstring()); } else { this->SetCreationDate(static_cast<const wchar_t*>(nullptr)); } }

		/// Sets all fields to "invalid".
		LIBCZI_API void Clear()
		{
			this->name_valid = this->title_valid = this->userName_valid = this->description_valid =
				this->comment_valid = this->keywords_valid = this->rating_valid = this->creationDateTime_valid = false;
		}
	private:
		void SetMember(std::wstring(GeneralDocumentInfo::* strMember), bool(GeneralDocumentInfo::* validMember), const wchar_t* str)
		{
			if (str == nullptr || *str == L'\0')
			{
				(this->*strMember).clear();
				this->*validMember = false;
			}
			else
			{
				this->*strMember = str;
				this->*validMember = true;
			}
		}
	};

	/// Scaling information - gives the size of a pixel.
	struct LIBCZI_API ScalingInfo
	{
		/// Default constructor - sets all members to invalid.
		ScalingInfo() : scaleX(std::numeric_limits<double>::quiet_NaN()), scaleY(std::numeric_limits<double>::quiet_NaN()), scaleZ(std::numeric_limits<double>::quiet_NaN()) {}
		double scaleX;	///< The length of a pixel in x-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().
		double scaleY;	///< The length of a pixel in y-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().
		double scaleZ;	///< The length of a pixel in y-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().

		/// Query if this object's scaleX value is valid.
		///
		/// \return True if this object's scaleX value is valid, false if not.
		bool IsScaleXValid() const { return !std::isnan(this->scaleX) && !std::isinf(this->scaleX); }

		/// Query if this object's scaleY value is valid.
		///
		/// \return True if this object's scaleY value is valid, false if not.
		bool IsScaleYValid() const { return !std::isnan(this->scaleY) && !std::isinf(this->scaleY); }

		/// Query if this object's scaleZ value is valid.
		///
		/// \return True if this object's scaleZ value is valid, false if not.
		bool IsScaleZValid() const { return !std::isnan(this->scaleZ) && !std::isinf(this->scaleZ); }

		/// Queries if the specified scale value is valid.
		///
		/// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
		///
		/// \param d Identifies the scale-value to query, can be 'x', 'y' or 'z' (or uppercase).
		///
		/// \return True if the specified scale is valid, false if not.
		bool IsScaleValid(char d) const
		{
			switch (d)
			{
			case 'x':case'X': return this->IsScaleXValid();
			case 'y':case'Y': return this->IsScaleYValid();
			case 'z':case'Z': return this->IsScaleZValid();
			default: throw std::invalid_argument("invalid dimension");
			}
		}

		/// Gets the specified scale value.
		///
		/// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
		///
		/// \param d Identifies the scale-value to query, can be 'x', 'y' or 'z' (or uppercase).
		///
		/// \return The specified scale.
		double GetScale(char d) const
		{
			switch (d)
			{
			case 'x':case'X': return this->scaleX;
			case 'y':case'Y': return this->scaleY;
			case 'z':case'Z': return this->scaleZ;
			default: throw std::invalid_argument("invalid dimension");
			}
		}
	};

	/// Extends the scaling information by a "default unit format". Note that the value itself remains to be
	/// given in meter, here we just suggest a unit to display the scale.
	struct ScalingInfoEx : ScalingInfo
	{
		std::wstring defaultUnitFormatX;	///< The default unit-format for X.
		std::wstring defaultUnitFormatY;	///< The default unit-format for Y.
		std::wstring defaultUnitFormatZ;	///< The default unit-format for Z.

		/// Gets the specified default unit-format.
		///
		/// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
		///
		/// \param d Identifies the scale-value to query, can be 'x', 'y' or 'z' (or uppercase).
		///
		/// \return The specified scale.
		LIBCZI_API std::wstring GetDefaultUnitFormat(char d) const
		{
			switch (d)
			{
			case 'x':case'X': return this->defaultUnitFormatX;
			case 'y':case'Y': return this->defaultUnitFormatY;
			case 'z':case'Z': return this->defaultUnitFormatZ;
			default: throw std::invalid_argument("invalid dimension");
			}
		}
	};

	/// Base class for information about the dimension. 
	class LIBCZI_API IDimensionInfo
	{
	public:

		/// Gets the dimension index.
		/// \return The dimension index.
		virtual DimensionIndex GetDimension() const = 0;

		/// Gets the interval.
		/// \param [out] start If non-null, it will receive the start index.
		/// \param [out] end   If non-null, it will receive the end index.
		virtual void GetInterval(int* start, int* end) const = 0;

		virtual ~IDimensionInfo() {}
	};

	/// This structure defines the information for the "Z-dimension". It resembles the ZEN-metadata-structure "Dimensions/Z".
	class LIBCZI_API IDimensionZInfo
	{
	public:
		///  Specifies the chirality of the coordinate system defined by
		///   - X axis in a subblock, from left to right
		///   - Y axis in a subblock, from top to bottom(-> origin in the top-left corner)
		///   - Z axis given by Z-index
		///  This chirality is used when arranging the z-slices spatially according to their Z-index.
		///  It is recommended to ensure that the chirality(and the order of the z-slices) is unchanged
		///  when arranging the z-slices according to their associated distances(as given by ../Dimensions/Z/Positions).
		enum class XyzHandedness : std::uint8_t
		{
			LeftHanded,	///< The coordinate-system is orientated left-handed.
			RightHanded,///< The coordinate-system is orientated right-handed.
			Undefined	///< The coordinate-system is undefined.
		};

		///  We define the z-axis to be collinear with the optical axis. On this axis the z-coordinates
	    ///  of the focal plane are measured, and their distances are found in the Z-labels
	    ///  (defined under ../Dimensions/Z/Positions) and the FocusPosition-field found in subblock-metadata.
	    ///  The direction of the axis may either be “from specimen to objective” or “from objective to specimen”.
	    ///  In this coordinate system the specimen is at a fixed position.Note that the origin of the coordinate
	    ///  system is _not_ defined here.
	    ///  Possible values are: FromSpecimenToObjective, FromObjectiveToSpecimen and undefined.
	    ///  Undefined is the default (and to be assumed if this element is not present).
	    ///  Note that the coordinate system defined here is different to the one defined with "XYZHandedness".
		enum class ZaxisDirection : std::uint8_t
		{
			FromSpecimenToObjective,	///< The z-axis is pointing from specimen to objective.
			FromObjectiveToSpecimen,	///< The z-axis is pointing from objective to specimen.
			Undefined					///< This parameter is undefined.
		};

		/// Description of mode of operation of the Z-Drive. "Continuous" means that
		///	the z - drive did not stop while acquiring the image. "Step" means that the
		///	Z - drive did not move during the acquisition of the image.
		///	Used for SPIM.
		enum class ZDriveMode : std::uint8_t 
		{
			Continuous,	///< The z-drive did not stop while acquiring the image.
			Step		///< The z-drive did not move during the acquisition of the image.
		};

		/// Gets the reference position in units of µm. If this is not valid, then the Z-positions only have a relative meaning.
		/// \param [in,out] d If non-null, the double will receive the requested value if successful.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetReferencePosition(double* d) = 0;

		/// Attempts to get interval definition for the z-positions. 
		/// \param [in,out] offset    If non-null and the method was successful, the offset is stored here.
		/// \param [in,out] increment If non-null and the method was successful, the increment is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetIntervalDefinition(double* offset, double* increment) = 0;

		/// Attempts to get list definition for the z-positions. 
		/// \param [in,out] positions    If non-null and the method was successful, the list of positions is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetPositionList(std::vector<double>* positions) = 0;

		/// Attempts to get XYZ-handedness property.
		/// \param [in,out] xyzHandedness If non-null and the method was successful, the XYZ-handedness is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetXyzHandedness(XyzHandedness* xyzHandedness) = 0;

		/// Attempts to get Z-axis direction property.
		/// \param [in,out] zAxisDirection If non-null and the method was successful, the Z-axis direction is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetZAxisDirection(ZaxisDirection* zAxisDirection) = 0;

		/// Attempts to get Z-drive mode property.
		/// \param [in,out] zdrivemode If non-null and the method was successful, the Z-drive mode is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetZDriveMode(ZDriveMode* zdrivemode) = 0;

		/// Attempts to get the z-drive speed (in units of µm/s).
		/// \param [in,out] zdrivespeed If non-null and the method was successful, the z-drive speed is stored here.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryZDriveSpeed(double* zdrivespeed) = 0;
	};

	/// This structure defines the information for the "T-dimension". It resembles the ZEN-metadata-structure "Dimensions/T".
	class LIBCZI_API IDimensionTInfo
	{
	public:
		/// Attempts to get start time - an absolute point in time which gives the reference point
		/// (to which to relative time spans refer to).
		/// \param [in,out] dateTime If non-null, the reference date time.
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetStartTime(XmlDateTime* dateTime) = 0;

		/// Attempts to get interval definition which allows to associate a time span with a t-index.
		/// \param [in,out] offset    If non-null, the offset will be put here (unit: second).
		/// \param [in,out] increment If non-null, the increment will be put here (unit: second).
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetIntervalDefinition(double* offset, double* increment) = 0;

		/// Attempts to get the offset list.
		/// \param [in,out] offsets If non-null, the offsets will be put here  (unit: second).
		/// \returns True if it succeeds, false if it fails.
		virtual bool TryGetOffsetsList(std::vector<double>* offsets) = 0;
	};

	class IChannelDisplaySetting;
	struct DisplaySettingsPOD;
	struct ChannelDisplaySettingsPOD;

	/// The display settings.
	class LIBCZI_API IDisplaySettings
	{
	public:
		/// The (normalized) control points of a spline.
		struct SplineControlPoint
		{
			double x;	///< The normalized x-coordinate of a spline control point.
			double y;	///< The normalized y-coordinate of a spline control point.

            /// Initializes a new instance of the SplineControlPoint class.
            /// \param x The x coordinate.
            /// \param y The y coordinate.
			SplineControlPoint(double x, double y) : x(x), y(y) {};

			/// Equality operator
			/// \param rhs The right hand side.
			/// \returns True if the parameters are considered equivalent.
			bool operator==(const SplineControlPoint& rhs) const { return this->x == rhs.x && this->y == rhs.y; }

			/// Inequality operator
			/// \param rhs The right hand side.
			/// \returns True if the parameters are not considered equivalent.
			bool operator!=(const SplineControlPoint& rhs) const { return !this->operator==(rhs); }
		};

		/// Values that represent the gradation curve modes.
		enum class GradationCurveMode : std::uint8_t
		{
			Linear,		///< The gradation curve is a straight line (from white point to black point).
			Gamma,		///< The gradation curve is defined by a gamma.
			Spline		///< The gradation curve is defined by piecewise splines.
		};


		/// This enum specifies the "tinting-mode" - how the channel is false-colored.
		/// \remark
		/// Plan is to add a property "GetTintingMode", currently we only implement "Color" and "None", so this information
		/// is conveniently contained in the method "TryGetTintingColorRgb8".
 		enum class TintingMode : std::uint8_t
		{
			None = 0,					///< None - which gives the "original color", ie. in case of RGB the RGB-value is directly used, in case of grayscale we get a gray pixel.
			Color = 1,					///< The pixel value is multiplied with the tinting-color.
			LookUpTableExplicit = 2,	///< (NOT YET IMPLEMENTED) There is an explicit look-up-table specified.
			LookUpTableWellKnown = 3	///< (NOT YET IMPLEMENTED) We are using a "well-known" look-up-table, and it is identified by its name (which is a string).
		};

		/// The coefficients of a cubic spline defined by \f$a\,x^3 + b\,x^2 + c\,x + d =y\f$.
		struct CubicSplineCoefficients
		{
			double a; ///< The coefficient of the cube.
			double b; ///< The coefficient of the square.
			double c; ///< The coefficient of the linear term.
			double d; ///< The constant.

			/// Gets the coefficients by an index (where a is 0, b is 1, c is 2 and d is 3). If the index is out-of-range, the method returns NaN.
			///
			/// \param index The index of the coefficient to get. 
			///
			/// \return The specified coefficient if the index is valid, NaN otherwise.
			double Get(int index) const
			{
				switch (index)
				{
				case 0:		return this->a;
				case 1:		return this->b;
				case 2:		return this->c;
				case 3:		return this->d;
				default:	return std::numeric_limits<double>::quiet_NaN();
				}
			}
		};

		/// The defintion of the (piecewise) spline. The spline starts at <tt>xPos</tt> which is the normalized position (between 0 and 1).
		struct SplineData
		{
			/// The (normalized) position for which this spline definition is valid.
			double xPos;

			/// The spline coefficients for this piece.
			CubicSplineCoefficients coefficients;
		};

		/// Enum all channels (which are described by the display-settings object).
		///
		/// \param func The functor to be called (passing in the channel index). If the functor returns false, the 
		/// 			enumeration is stopped.
		virtual void EnumChannels(std::function<bool(int chIndex)> func) const = 0;

		/// Gets channel display settings for the specified channel. If the channel index is not valid, then
		/// an empty shared_ptr is returned.
		///
		/// \param chIndex The channel index.
		///
		/// \return The channel display settings object (if the channel index was valid), and empty shared_ptr otherwise.
		virtual std::shared_ptr<libCZI::IChannelDisplaySetting> GetChannelDisplaySettings(int chIndex) const = 0;

		virtual ~IDisplaySettings() {}

		/// Makes a deep copy of the information in this object and store the information in the POD.
		/// \param 		    disp The display-settings object.
		/// \param [in,out] pod  The POD-display-settings object to store the information in.
		static void Clone(const IDisplaySettings* disp, DisplaySettingsPOD& pod);
	};

	/// The display-settings for a channel.
	class LIBCZI_API IChannelDisplaySetting
	{
	public:
		/// Gets a boolean indicating whether the corresponding channel is 'active' in the multi-channel-composition.
		///
		/// \return True if the corresponding channel is 'active', false otherwise.
		virtual bool	GetIsEnabled() const = 0;

		/// Gets the weight of the channel (for multi-channel-composition).
		///
		/// \return The weight.
		virtual float	GetWeight() const = 0;

		/// Attempts to get the RGB24-tinting color for the corresponding channel. If tinting is not enabled, then
		/// this method will return false.
		///
		/// \param [out] pColor If tinting is enabled for the corresponding channel, then (if non-null) will receive the tinting-color.
		///
		/// \return True if tinting is enabled for the corresponding channel (and in this case <tt>pColor</tt> will be set), false otherwise (and <tt>pColor</tt> will not be set).
		virtual bool	TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const = 0;

		/// Gets the black point and the white point.
		///
		/// \param [out] pBlack If non-null, the black point will be returned.
		/// \param [out] pWhite If non-null, the white point will be returned.
		virtual void	GetBlackWhitePoint(float* pBlack, float* pWhite) const = 0;

		/// Gets gradation curve mode.
		///
		/// \return The gradation curve mode.
		virtual IDisplaySettings::GradationCurveMode GetGradationCurveMode() const = 0;

		/// Attempts to get the gamma - this will only be available if gradation curve mode is <tt>Gamma</tt>.
		///
		/// \param [out] gamma If non-null and applicable, the gamma will be returned.
		///
		/// \return True if the corresponding channel uses gradation curve mode <tt>Gamma</tt> (and a value for gamma is available), false otherwise.
		virtual bool	TryGetGamma(float* gamma)const = 0;

		/// Attempts to get spline control points - this will only be available if gradation curve mode is <tt>Spline</tt>.
		/// \remark
		/// We make no promises that both the control-points and the spline-data are always available. It might be plausible that
		/// the spline is defined in a different way (different than control-points), so in that case only the "spline-data" would
		/// be available. So - be careful is using this interface in a context different thant "CZI-metadata" where it might be
		/// the case the 'TryGetSplineControlPoints' will fail but 'TryGetSplineData' might succeed.
		/// Maybe better should remove 'TryGetSplineData' from this interface.
		///
		/// \param [in,out] ctrlPts If non-null, the control points will be written to this vector.
		///
		/// \return True if it succeeds, false if it fails.
		virtual bool	TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const = 0;

		/// Attempts to get the spline data - this will only be available if gradation curve mode is <tt>Spline</tt>.
		///
		/// \param [in,out] data If non-null, the spline data will be written to this vector.
		///
		/// \return True if it the corresponding channels uses gradation curve mode <tt>Spline</tt>, false otherwise.
		virtual bool	TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const = 0;

		virtual ~IChannelDisplaySetting() {}

		/// Makes a deep copy of the information in this object and store the information in the POD.
		/// \param 		    disp The channel-display-settings object.
		/// \param [in,out] pod  The POD-channel-display-settings object to store the information in.
		static void Clone(const IChannelDisplaySetting* disp, ChannelDisplaySettingsPOD& pod);
	};

	/// This POD ("plain-old-data") structure is intended to capture all information found inside an
	/// IChannelDisplaySetting-object. It allows for easy modification of the information.
	struct ChannelDisplaySettingsPOD
	{
		/// A boolean indicating whether the corresponding channel is 'active' in the multi-channel-composition.
		bool isEnabled;

		/// The weight of the channel (for multi-channel-composition).
		float weight;
		
		/// The tinting mode.
		IDisplaySettings::TintingMode tintingMode;

		/// The tinting color (only valid if tinting mode == Color).
		libCZI::Rgb8Color tintingColor;

		/// The (normalized) black point value.
		float blackPoint;

		/// The (normalized) white point value.
		float whitePoint;

		/// The gradation curve mode.
		IDisplaySettings::GradationCurveMode  gradationCurveMode;

		/// The value for gamma (only valid if gradation curve mode == Gamma).
		float gamma;

		/// The spline control points  (only valid if gradation curve mode == Spline).
		std::vector<libCZI::IDisplaySettings::SplineControlPoint> splineCtrlPoints;

		/// Creates an IChannelDisplaySetting-object from the specified ChannelDisplaySettingsPOD-structure.
		/// \param pod The ChannelDisplaySettingsPOD-structure.
		/// \returns The newly created IChannelDisplaySetting-object.
		LIBCZI_API static IChannelDisplaySetting* CreateIChannelDisplaySetting(const ChannelDisplaySettingsPOD& pod);

		/// Creates an IChannelDisplaySetting-object from the specified ChannelDisplaySettingsPOD-structure, and return it
		/// as a shared pointer.
		/// \param pod The ChannelDisplaySettingsPOD-structure.
		/// \returns The newly created IChannelDisplaySetting-object inside a shared pointer.
		LIBCZI_API static std::shared_ptr<IChannelDisplaySetting> CreateIChannelDisplaySettingSp(const ChannelDisplaySettingsPOD& pod);

		/// Sets the structure to a defined standard value - not enabled, no tinting, linear gradation-curve and 
		/// black-point to zero and white-point to one.
		LIBCZI_API void Clear()
		{
			this->isEnabled = false;
			this->tintingMode = IDisplaySettings::TintingMode::None;
			this->blackPoint = 0;
			this->whitePoint = 1;
			this->gradationCurveMode = IDisplaySettings::GradationCurveMode::Linear;
			this->weight = 1;
			this->gamma = 1;
		}
	};

	/// This POD ("plain-old-data") structure is intended to capture all information found inside an
	/// IChannelDisplaySetting-object. It allows for easy modification of the information.
	struct DisplaySettingsPOD
	{
		/// The channel display settings. Key is the channel index, and value is the POD-channel-display-data structure.
		std::map<int, ChannelDisplaySettingsPOD > channelDisplaySettings;

		/// Creates an IDisplaySettings-object from the specified DisplaySettingsPOD-structure.
		/// \param pod The ChannelDisplaySettingsPOD-structure.
		/// \returns The newly created IDisplaySettings-object.
		LIBCZI_API static libCZI::IDisplaySettings* CreateIDisplaySetting(const DisplaySettingsPOD& pod);

		/// Creates an IDisplaySettings-object from the specified DisplaySettingsPOD-structure, and return it
		/// as a shared pointer.
		/// \param pod The ChannelDisplaySettingsPOD-structure.
		/// \returns The newly created IDisplaySettings-object inside a shared pointer.
		LIBCZI_API static std::shared_ptr<libCZI::IDisplaySettings> CreateIDisplaySettingSp(const DisplaySettingsPOD& pod);
	};

	class IDimensionsChannelsInfo;

	/// The top-level interface for the CZI-metadata object.
	class LIBCZI_API ICziMultiDimensionDocumentInfo
	{
	public:
		/// Gets "general document information".
		/// \return The "general document information".
		virtual GeneralDocumentInfo GetGeneralDocumentInfo() const = 0;

		/// Gets "extended scaling information".
		/// \return The "extended scaling information".
		virtual libCZI::ScalingInfoEx GetScalingInfoEx() const = 0;

		/// Gets "scaling information".
		/// \return The "scaling information".
		virtual libCZI::ScalingInfo GetScalingInfo() const = 0;

		/// Enumerate the dimensions (defined in the metadata under Metadata/Information/Image, checking for the nodes
		/// StartZ, SizeZ, StartC, SizeC, StartT, StartT, ...).
		/// \remark
		/// The information here is not considered authoritative.
		/// \param enumDimensions The functor which will be called for each dimension. If the functor returns false, the enumeration is canceled.
		virtual void EnumDimensions(std::function<bool(DimensionIndex)> enumDimensions) = 0;

		/// Gets the dimension information for the specified dimension.
		/// \remark
		/// The information here is not considered authoritative.
		/// \param dim The dimension to retrieve the information for.
		/// \return The dimension information if available.
		virtual std::shared_ptr<IDimensionInfo> GetDimensionInfo(DimensionIndex dim) = 0;

		/// Gets information about the "dimension z" (from the Dimension/Z metadata node). If this node is not
		/// available, an empty pointer is returned.
		/// \returns The "dimension z" information.
		virtual std::shared_ptr<IDimensionZInfo> GetDimensionZInfo() = 0;

		/// Gets information about the "dimension t" (from the Dimension/T metadata node). If this node is not
		/// available, an empty pointer is returned.
		/// \returns The "dimension t" information.
		virtual std::shared_ptr<IDimensionTInfo> GetDimensionTInfo() = 0;

        /// Gets information about the "dimension c" (from the Dimension/Channels metadata node). If this node is not
        /// available, an empty pointer is returned.
        /// \returns The dimension channels information.
		virtual std::shared_ptr<IDimensionsChannelsInfo> GetDimensionChannelsInfo() = 0;

		/// Gets the display settings.
		/// \remark
		/// This method may return an empty shared_ptr in case that display-settings are not present in the metadata.
		/// \return The display settings object.
		virtual std::shared_ptr<IDisplaySettings> GetDisplaySettings() const = 0;

		virtual ~ICziMultiDimensionDocumentInfo() {}

		/// Gets a vector with all dimensions (found in metadata).
		/// \return The vector containing all dimensions.
		std::vector<DimensionIndex> GetDimensions()
		{
			std::vector<DimensionIndex> vec;
			this->EnumDimensions([&](DimensionIndex i)->bool {vec.push_back(i); return true; });
			return vec;
		}
	};

	/// This interface provides read-only access to an XML-node.
	class LIBCZI_API IXmlNodeRead
	{
	public:
		/// Gets the name of the node.
		/// \returns The name of the node.
		virtual std::wstring Name() const = 0;

		/// Attempts to get the attribute with the name specified by "attributeName". If it exists, the value is stored in
		/// "attribValue" (if it is non-null) and the return value is true. Otherwise, the return value is false.
		///
		/// \param 		    attributeName Name of the attribute.
		/// \param [out] attribValue   If non-null, the attribute value will be put here (if the attribute exists).
		///
		/// \return True if it succeeds, false if it fails.
		virtual bool TryGetAttribute(const wchar_t* attributeName, std::wstring* attribValue) const = 0;

		/// Enumerate the attributes in the node. The attribute-name and their respective value will be passed to the specified functor.
		///
		/// \param enumFunc The enumeration function. If the function returns false, the enumeration will be cancelled immediately.
		virtual void EnumAttributes(const std::function<bool(const std::wstring& attribName, const std::wstring& attribValue)>& enumFunc) const = 0;

		/// Attempts to get value of the XML-node. If the specified pointer "value" is non-null, the value will be put there.
		///
		/// \param [out] value If non-null, the value of the XML-node will be put here (if successful).
		///
		/// \return True if it succeeds, false if it fails.
		virtual bool TryGetValue(std::wstring* value) const = 0;

		/// Gets a child node for the specified path/attribute specification if it
		/// exists. Otherwise, a nullptr is returned. 
		/// The path is specified as node-names separated by slashes.
		/// At path "A/B/C" selects (or creates) a node-structure like this
		/// \code{.unparsed}
		/// <A>
		///	  <B>
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// Attributes can be specified with a node, in the form 'NodeName[attr1=abc,attr2=xyz]'. This will
		/// search for nodes with the specified attributes, and if not found, create one.
		/// In this example "A/B[Id=ab,Name=xy]/C" we will get
		/// \code{.unparsed}
		/// <A>
		///	  <B Id="ab" Name="xy">
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// \param path The path  (in UTF8-encoding).
		/// \return Either the requested node if it exists or nullptr.
		virtual std::shared_ptr<IXmlNodeRead> GetChildNodeReadonly(const char* path) = 0;

		/// Enumerate the children of the node. The functor will be called until there are no
		/// more children or if the function return false.
		/// \param enumChildren The function to be called with the children nodes.
		virtual void EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren) = 0;

		/// Attempts to get value of this node as a double. If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to a double to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsDouble(double* p);

		/// Attempts to get value of this node as a float. If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to a float to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsFloat(float* p);

		/// Attempts to get value of this node as an integer (32-bit). If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to an integer to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsInt32(std::int32_t* p);

		/// Attempts to get value of this node as an unsigned integer (32-bit). If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to an unsigned integer to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsUInt32(std::uint32_t* p);

		/// Attempts to get value of this node as a long integer (64-bit). If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to a long integer (64-bit) to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsInt64(std::int64_t* p);

		/// Attempts to get value of this node as an unsigned long integer (64-bit). If the text does not parse correctly (or is empty), we return false.
		/// \param [in,out] p If non-null, a pointer to an unsigned long integer (64-bit) to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsUInt64(std::uint64_t* p);

		/// Attempts to get value of this node as an unsigned long integer (64-bit). Valid values for true are: "true", "yes" or "1";
		/// valid values for false are "false", "no" or "0" (case insensitive comparison).
		/// \param [in,out] p If non-null, a pointer to a bool to store the result in.
		/// \returns True if it succeeds, false if it fails.
		bool TryGetValueAsBool(bool* p);

		virtual ~IXmlNodeRead() {};
	};

	/// Representation of the CZI-metadata.
	class LIBCZI_API ICziMetadata : public IXmlNodeRead
	{
	public:
		/// Query if the CZI's metadata (the XML) is well-formed and parsed correctly.
		/// If this is not the case, then other methods (of this interface) will thrown a
		/// LibCZIXmlParseException exception.
		/// 
		/// \returns True if the XML is well-formed, false if not.
		virtual bool IsXmlValid() const = 0;

		/// Gets the metadata as an unprocessed UTF8-encoded XML-string. 
		///
		/// \return The metadata (unprocessed UTF8-encoded XML).
		virtual std::string GetXml() = 0;

		/// Gets the "document information" part of the metadata.
		///
		/// \return The "document information".
		virtual std::shared_ptr<libCZI::ICziMultiDimensionDocumentInfo> GetDocumentInfo() = 0;

		virtual ~ICziMetadata() {}
	};

	class IXmlNodeRw;

	/// This interface provides write access to an XML-node.
	class LIBCZI_API IXmlNodeWrite
	{
	public:
		/// Gets or create a child node. The path is specified as node-names separated by slashes.
		/// At path "A/B/C" selects (or creates) a node-structure like this
		/// \code{.unparsed}
		/// <A>
		///	  <B>
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// Attributes can be specified with a node, in the form 'NodeName[attr1=abc,attr2=xyz]'. This will
		/// search for nodes with the specified attributes, and if not found, create one.
		/// In this example "A/B[Id=ab,Name=xy]/C" we will get
		/// \code{.unparsed}
		/// <A>
		///	  <B Id="ab" Name="xy">
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// \param path The path  (in UTF8-encoding).
		/// \return Either an existing node or a newly created one.
		virtual std::shared_ptr<IXmlNodeRw> GetOrCreateChildNode(const char* path) = 0;

		/// Gets an existing child node. The path is specified as node-names separated by slashes.
		/// At path "A/B/C" selects (or creates) a node-structure like this
		/// \code{.unparsed}
		/// <A>
		///	  <B>
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// Attributes can be specified with a node, in the form 'NodeName[attr1=abc,attr2=xyz]'. This will
		/// search for nodes with the specified attributes, and if not found, create one.
		/// In this example "A/B[Id=ab,Name=xy]/C" we will get
		/// \code{.unparsed}
		/// <A>
		///	  <B Id="ab" Name="xy">
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// \param path The path (in UTF8-encoding).
		/// \return The existing node conforming to the path if it exists, null otherwise.
		virtual std::shared_ptr<IXmlNodeRw> GetChildNode(const char* path) = 0;

		/// Appends a child node with the specified name.
		///
		/// \param name The name of the node to add.
		///
		/// \return The newly added node.
		virtual std::shared_ptr<IXmlNodeRw> AppendChildNode(const char* name) = 0;

		/// Sets the attribute with the specified name to the specified value.
		///
		/// \param name  The name of the attribute (as UTF8-encoded string).
		/// \param value The value (as UTF8-encoded string).
		virtual void SetAttribute(const char* name, const char* value) = 0;

		/// Sets the node value - which is specified as an UTF8-string.
		///
		/// \param str The UTF8-encoded string.
		virtual void SetValue(const char* str) = 0;

		/// Sets the node value to the specified string.
		///
		/// \param str The string.
		virtual void SetValue(const wchar_t* str) = 0;

		/// Sets value of the node with the specified integer.
		///
		/// \param value The integer to write into the node.
		virtual void SetValueI32(int value) = 0;

		/// Sets value of the node with the specified unsigned integer.
		///
		/// \param value The unsigned integer to write into the node.
		virtual void SetValueUI32(unsigned int value) = 0;

		/// Sets value of the node with the specified double.
		///
		/// \param value The double to write into the node.
		virtual void SetValueDbl(double value) = 0;

		/// Sets value of the node with the specified float.
		///
		/// \param value The float to write into the node.
		virtual void SetValueFlt(float value) = 0;

		/// Sets value of the node with the specified boolean.
		///
		/// \param value The boolean to write into the node.
		virtual void SetValueBool(bool value) = 0;

		/// Sets value of the node with the specified long integer.
		///
		/// \param value The long integer to write into the node.
		virtual void SetValueI64(long long value) = 0;

		/// Sets value of the node with the specified unsigned long integer.
		///
		/// \param value The unsigned long integer to write into the node.
		virtual void SetValueUI64(unsigned long long value) = 0;

		virtual ~IXmlNodeWrite() {};

		/// Sets the node value - which is specified as an UTF8-string.
		///
		/// \param str The UTF8-encoded string.
		void SetValue(const std::string& str) { this->SetValue(str.c_str()); }

		/// Sets the node value to the specified string.
		///
		/// \param str The string.
		void SetValue(const std::wstring& str) { this->SetValue(str.c_str()); }

		/// Gets or create a child node. The path is specified as node-names separated by slashes.
		/// At path "A/B/C" selects (or creates) a node-structure like this
		/// \code{.unparsed}
		/// <A>
		///	  <B>
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// Attributes can be specified with a node, in the form 'NodeName[attr1=abc,attr2=xyz]'. This will
		/// search for nodes with the specified attributes, and if not found, create one.
		/// In this example "A/B[Id=ab,Name=xy]/C" we will get
		/// \code{.unparsed}
		/// <A>
		///	  <B Id="ab" Name="xy">
		///     <C/>
		///	  </B>
		///	</A>
		/// \endcode
		/// \param path The path.
		/// \return Either an existing node or a newly created one.
		std::shared_ptr<IXmlNodeRw> GetOrCreateChildNode(const std::string& path) { return this->GetOrCreateChildNode(path.c_str()); }

		/// Gets a child node for the specified path.
		/// \param path Path of the childnode (in UTF8-encoding).
		/// \return The child node.
		std::shared_ptr<IXmlNodeRw> GetChildNode(const std::string& path) { return this->GetChildNode(path.c_str()); }
	};

	/// This interface combines read- and write-access for an XML-node.
	class IXmlNodeRw : public IXmlNodeRead, public IXmlNodeWrite
	{
	};

	/// A utility class intended for constructing CZI-metadata.
	class ICziMetadataBuilder
	{
	public:
		/// Gets the root node.
		///
		/// \return The root node.
		virtual std::shared_ptr<IXmlNodeRw> GetRootNode() = 0;

		/// Gets the metadata as an UTF8-encoded XML-string.
		///
		/// \return The metadata (UTF8-encoded XML).
		virtual std::string GetXml(bool withIndent = false) = 0;

		virtual ~ICziMetadataBuilder() {};
	};

	/// Helper functions useful for constructing CZI-metadata.
	class LIBCZI_API MetadataUtils
	{
	public:
		/// Writes the nodes ""Metadata/Information/Image/SizeX" and ""Metadata/Information/Image/SizeY".
		///
		/// \param [in,out] builder The metadata-builder object.
		/// \param 		    width   The width (=SizeX).
		/// \param 		    height  The height (=SizeY).
		static void WriteImageSizeInformation(libCZI::ICziMetadataBuilder* builder, int width, int height);

		/// Writes the node ""Metadata/Information/Image/SizeM".
		///
		/// \param [in,out] builder The metadata-builder object.
		/// \param 		    mSize   The M-size.
		static void WriteMIndexSizeInformation(libCZI::ICziMetadataBuilder* builder, int mSize);

		/// Writes the size-information for the specified dimension (to "Metadata/Information/Image/Size?").
		///
		/// \param [in,out] builder The metadata-builder object.
		/// \param 		    dim	    The dimension.
		/// \param 		    size    The size of the dimension.
		static void WriteDimensionSize(libCZI::ICziMetadataBuilder* builder, libCZI::DimensionIndex dim, int size);

		/// Uses the specified statistics-data in order to write the "size"-information-
		///
		/// \param [in,out] builder    The metadata-builder object.
		/// \param 		    statistics The subblock-statistics.
		static void WriteFillWithSubBlockStatistics(libCZI::ICziMetadataBuilder* builder, const libCZI::SubBlockStatistics& statistics);

		/// Writes the "dimension-T"-information ("Information/Dimensions/T") - this associates a timestamp with each T-coordinate. With this
		/// function we provide the timestamps for the equidistant case (= the difference between consecutive T-indices is constant).
		/// The difference is specified as "increment". It is possible to specify an offset for the first T-index, given as startOffset.
		/// An absolute time-point may be specified as startTime. 
		/// Example: say, startOffSet is 1 and increment is 2. Then T=0 is associated with 1, T=1 with 1+2, T=2 with 1+2*2, T=3 with 1+2*3 and so on.
		/// 
		/// \param [in,out] builder	    The metadata-builder object.
		/// \param 		    startTime   If non-null, the (absolute) start time.
		/// \param 		    startOffSet The offset for the first timepoint (in units of seconds). If numeric_limits<double>::quiet_NaN() is specified, this parameter is ignored.
		/// \param 		    increment   The increment (in units of seconds). If numeric_limits<double>::quiet_NaN() is specified, this parameter is ignored.
		static void WriteDimInfoT_Interval(libCZI::ICziMetadataBuilder* builder, const libCZI::XmlDateTime* startTime, double startOffSet, double increment);

		/// Writes the "dimension-T"-information ("Information/Dimensions/T") - this associates a timestamp with each T-coordinate. With this function
		/// we provide a list of timestamps. 
		/// 
		/// \param [in,out] builder		   The metadata-builder object.
		/// \param 		    startTime	   If non-null, the (absolute) start time.
		/// \param 		    funcGetOffsets A function which is called for providing the timestamp. The integer parameter is incremented with each call,
		/// 							   starting with 0. The return value is the timestamp (in units of seconds). If numeric_limits<double>::quiet_NaN() is
		/// 							   returned, the enumeration is ended.
		static void WriteDimInfoT_List(libCZI::ICziMetadataBuilder* builder, const libCZI::XmlDateTime* startTime, std::function<double(int)> funcGetOffsets);

		/// Helper function in order to write the Dimension-Z information - as a an equal-distance sequence.
		///
		/// \param [in] builder			The metadata-builder object.
		/// \param 		    startPos    The start position.
		/// \param 		    startOffSet The offset of the first item in the sequence.
		/// \param 		    increment   The increment.
		static void WriteDimInfoZ_Interval(libCZI::ICziMetadataBuilder* builder, double startPos, double startOffSet, double increment);

		/// Helper function in order to write the Dimension-Z information - as a an explicit list.
		///
		/// \param [in] builder		   The metadata-builder object.
		/// \param 	    startPos	   The start position.
		/// \param 	    funcGetOffsets The function which is called to retrieve the offset. The argument is incremented with each call (starting with 0);
		/// 						   a return value of infinity or NaN will end the enumeration.
		static void WriteDimInfoZ_List(libCZI::ICziMetadataBuilder* builder, double startPos, std::function<double(int)> funcGetOffsets);

		/// Helper function which writes the information from the specified "GeneralDocumentInfo" into the metadata-builder object.
		///
		/// \param [in] builder		The metadata-builder object.
		/// \param 		    info    The general-document-information to be written into the metadata-builder object.
		static void WriteGeneralDocumentInfo(libCZI::ICziMetadataBuilder* builder, const libCZI::GeneralDocumentInfo& info);

		/// Helper function in order to write scaling-information into the metadata-builder object.
		///
		/// \param [in] builder		The metadata-builder object.
		/// \param 		scalingInfo The scaling  to be written into the metadata-builder object.
		static void WriteScalingInfo(libCZI::ICziMetadataBuilder* builder, const libCZI::ScalingInfo& scalingInfo);

		/// Helper function in order to write scaling-information into the metadata-builder object.
		///
		/// \param [in] builder		The metadata-builder object.
		/// \param 		scalingInfo The scaling  to be written into the metadata-builder object.
		static void WriteScalingInfoEx(libCZI::ICziMetadataBuilder* builder, const libCZI::ScalingInfoEx& scalingInfo);
	};
}
