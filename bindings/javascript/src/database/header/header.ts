import { DwgPoint3D } from '../common'

export interface DwgHeader {
  /**
   * The AutoCAD drawing database version number:
   * - AC1006: R10
   * - AC1009: R11 and R12
   * - AC1012: R13
   * - AC1014: R14
   * - AC1015: AutoCAD 2000
   * - AC1018: AutoCAD 2004
   * - AC1021: AutoCAD 2007
   * - AC1024: AutoCAD 2010
   * - AC1027: AutoCAD 2013
   * - AC1032: AutoCAD 2018
   */
  ACADVER?: string

  /**
   * The zero (0) base angle with respect to the current UCS.
   * Initial value:	0.0000
   */
  ANGBASE?: number

  /**
   * The direction of positive angles
   * Initial value:	0
   * - 0: Counterclockwise angles
   * - 1: Clockwise angles
   */
  ANGDIR?: number

  /**
   * Attribute visibility
   * Initial value:	1
   * - 0: Off: Makes all attributes invisible
   * - 1: Normal: Retains current visibility of each attribute; visible attributes are displayed; invisible attributes are not
   * - 2: On: Makes all attributes visible
   */
  ATTMODE?: number

  /**
   * Units format for angles
   * Initial value:	0
   * - 0: Decimal degrees
   * - 1: Degrees/minutes/seconds
   * - 2: Gradians
   * - 3: Radians
   * - 4: Surveyor's units
   */
  AUNITS?: number

  /**
   * The display precision for angular units and coordinates. However, the internal precision of angular
   * values and coordinates is always maintained, regardless of the display precision. AUPREC does not
   * affect the display precision of dimension text (see DIMSTYLE). Valid values are integers from 0 to 8.
   * Initial value:	0
   */
  AUPREC?: number

  /**
   * Current entity color number
   * 0 = BYBLOCK; 256 = BYLAYER
   * Initial value:	256
   */
  CECOLOR?: number

  /**
   * The linetype scaling for new objects relative to the LTSCALE command setting. A line created with
   * CELTSCALE = 2 in a drawing with LTSCALE set to 0.5 would appear the same as a line created with
   * CELTSCALE = 1 in a drawing with LTSCALE = 1.
   * Initial value:	1.0000
   */
  CELTSCALE?: number

  /**
   * Entity linetype name, or BYBLOCK or BYLAYER
   * Initial value:	BYLAYER
   */
  CELTYPE?: string

  /**
   * The lineweight of new objects.
   * Initial value:	-1
   * - -1: Sets the lineweight to "BYLAYER."
   * - -2: Sets the lineweight to "BYBLOCK."
   * - -3: Sets the lineweight to "DEFAULT." "DEFAULT" is controlled by the LWDEFAULT system variable.
   */
  CELWEIGHT?: number

  /**
   * Plotstyle handle of new objects; if CEPSNTYPE is 3, then this value indicates the handle
   */
  CEPSNID?: number

  /**
   * Plot style type of new objects:
   * - 0: Plot style by layer
   * - 1: Plot style by block
   * - 2: Plot style by dictionary default
   * - 3: Plot style by object ID/handle
   */
  CEPSNTYPE?: number

  /**
   * The first chamfer distance
   * Initial value:	0.0000
   */
  CHAMFERA?: number

  /**
   * The second chamfer distance
   * Initial value:	0.0000
   */
  CHAMFERB?: number

  /**
   * Chamfer length
   */
  CHAMFERC?: number

  /**
   * Chamfer angle
   */
  CHAMFERD?: number

  /**
   * Current layer name
   * Initial value:	'0'
   */
  CLAYER?: string

  /**
   * Current multiline justification
   * Initial value:	0
   * - 0: Top
   * - 1: Middle
   * - 2: Bottom
   */
  CMLJUST?: number

  /**
   * Current multiline scale. A scale factor of 2.0 produces a multiline twice as wide as the style
   * definition. A zero scale factor collapses the multiline into a single line. A negative scale
   * factor flips the order of the offset lines (that is, the smallest or most negative is placed
   * on top when the multiline is drawn from left to right).
   * Initial value:	1.0000 (imperial) or 20.0000 (metric)
   */
  CMLSCALE?: number

  /**
   * The multiline style that governs the appearance of the multiline.
   * Initial value:	'Standard'
   */
  CMLSTYLE?: string

  /**
   * Shadow mode for a 3D object
   * - 0: Casts and receives shadows
   * - 1: Casts shadows
   * - 2: Receives shadows
   * - 3: Ignores shadows
   * Note: Starting with AutoCAD 2016-based products, this variable is obsolete but still supported for
   * backwards compatibility.
   */
  CSHADOW?: number

  /**
   * The number of precision places displayed in angular dimensions
   * Initial value:	0
   * - -1: Angular dimensions display the number of decimal places specified by DIMDEC
   * - 0-8: Specifies the number of decimal places displayed in angular dimensions (independent of DIMDEC)
   */
  DIMADEC?: number

  /**
   * The display of alternate units in dimensions.
   * Initial value:	0
   * - 0: Disables alternate units
   * - 1: Enables alternate units
   */
  DIMALT?: number

  /**
   * The number of decimal places in alternate units. If DIMALT is turned on, DIMALTD sets the number of
   * digits displayed to the right of the decimal point in the alternate measurement.
   * Initial value:	2 (imperial) or 3 (metric)
   */
  DIMALTD?: number

  /**
   * The multiplier for alternate units. If DIMALT is turned on, DIMALTF multiplies linear dimensions by
   * a factor to produce a value in an alternate system of measurement. The initial value represents the
   * number of millimeters in an inch.
   * Initial value:	25.4000 (imperial) or 0.0394 (metric)
   */
  DIMALTF?: number

  /**
   * Rounds off the alternate dimension units.
   * Initial value:	0.0000
   */
  DIMALTRND?: number

  /**
   * The number of decimal places for the tolerance values in the alternate units of a dimension.
   * Initial value:	2 (imperial) or 3 (metric)
   */
  DIMALTTD?: number

  /**
   * Controls suppression of zeros for alternate tolerance values.
   * Initial value:	0
   * - 0: Suppresses zero feet and precisely zero inches
   * - 1: Includes zero feet and precisely zero inches
   * - 2: Includes zero feet and suppresses zero inches
   * - 3: Includes zero inches and suppresses zero feet
   *
   * To suppress leading or trailing zeros, add the following values to one of the preceding values:
   * - 4: Suppresses leading zeros
   * - 8: Suppresses trailing zeros
   */
  DIMALTTZ?: number

  /**
   * Units format for alternate units of all dimension style family members except angular:
   * Initial value:	2
   * - 1: Scientific
   * - 2: Decimal
   * - 3: Engineering
   * - 4: Architectural (stacked)
   * - 5: Fractional (stacked)
   * - 6: Architectural
   * - 7: Fractional
   * - 8: Operating system defines the decimal separator and number grouping symbols
   */
  DIMALTU?: number

  /**
   * Controls suppression of zeros for alternate unit dimension values.
   * Initial value:	0
   * - 0: Suppresses zero feet and precisely zero inches
   * - 1: Includes zero feet and precisely zero inches
   * - 2: Includes zero feet and suppresses zero inches
   * - 3: Includes zero inches and suppresses zero feet
   * - 4: Suppresses leading zeros in decimal dimensions
   * - 8: Suppresses trailing zeros in decimal dimensions
   * - 12: Suppresses both leading and trailing zeros
   */
  DIMALTZ?: number

  /**
   * Specifies a text prefix or suffix (or both) to the alternate dimension measurement for all types of
   * dimensions except angular. For instance, if the current units are Architectural, DIMALT is on,
   * DIMALTF is 25.4 (the number of millimeters per inch), DIMALTD is 2, and DIMAPOST is set to "mm",
   * a distance of 10 units would be displayed as 10"[254.00mm].
   * To turn off an established prefix or suffix (or both), set it to a single period (.).
   * Initial value:	""
   */
  DIMAPOST?: number

  /**
   * 1 = Create associative dimensioning
   * 0 = Draw individual entities
   * Note: Obsolete; see DIMASSOC.
   */
  DIMASO?: number

  /**
   * Controls the associativity of dimension objects
   * Initial value:	2
   * - 0: Creates exploded dimensions; there is no association between elements of the dimension, and the lines, arcs, arrowheads, and text of a dimension are drawn as separate objects
   * - 1:  Creates non-associative dimension objects; the elements of the dimension are formed into a single object, and if the definition point on the object moves, then the dimension value is updated
   * - 2: Creates associative dimension objects; the elements of the dimension are formed into a single object and one or more definition points of the dimension are coupled with association points on geometric objects
   */
  DIMASSOC?: number

  /**
   * Controls the size of dimension line and leader line arrowheads. Also controls the size of hook lines.
   * Multiples of the arrowhead size determine whether dimension lines and text should fit between the
   * extension lines. DIMASZ is also used to scale arrowhead blocks if set by DIMBLK. DIMASZ has no effect
   * when DIMTSZ is other than zero.
   * Initial value:	0.1800 (imperial) or 2.5000 (metric)
   */
  DIMASZ?: number

  /**
   * Determines how dimension text and arrows are arranged when space is not sufficient to place both within
   * the extension lines.
   * Initial value:	3
   * - 0: Places both text and arrows outside extension lines
   * - 1: Moves arrows first, then text
   * - 2: Moves text first, then arrows
   * - 3: Moves either text or arrows, whichever fits best
   */
  DIMATFIT?: number

  /**
   * Angle format for angular dimensions
   * Initial value:	0
   * - 0: Decimal degrees
   * - 1: Degrees/minutes/seconds
   * - 2: Gradians
   * - 3: Radians
   * - 4: Surveyor's units
   */
  DIMAUNIT?: number

  /**
   * Suppresses zeros for angular dimensions
   * Initial value:	0
   * - 0: Displays all leading and trailing zeros
   * - 1: Suppresses leading zeros in decimal dimensions (for example, 0.5000 becomes .5000)
   * - 2: Suppresses trailing zeros in decimal dimensions (for example, 12.5000 becomes 12.5)
   * - 3: Suppresses leading and trailing zeros (for example, 0.5000 becomes .5)
   */
  DIMAZIN?: number

  /**
   * Sets the arrowhead block displayed at the ends of dimension lines.
   * To return to the default, closed-filled arrowhead display, enter a single period (.).
   * Arrowhead block entries and the names used to select them in the New, Modify, and Override
   * Dimension Style dialog boxes are shown below. You can also enter the names of user-defined
   * arrowhead blocks.
   * Initial value:	""
   * - "": closed filled
   * - "_DOT": dot
   * - "_DOTSMALL": dot small
   * - "_DOTBLANK": dot blank
   * - "_ORIGIN": origin indicator
   * - "_ORIGIN2": origin indicator 2
   * - "_OPEN": open
   * - "_OPEN90": right angle
   * - "_OPEN30": open 30
   * - "_CLOSED": closed
   * - "_SMALL": dot small blank
   * - "_NONE": none
   * - "_OBLIQUE": oblique
   * - "_BOXFILLED": box filled
   * - "_BOXBLANK": box
   * - "_CLOSEDBLANK": closed blank
   * - "_DATUMFILLED": datum triangle filled
   * - "_DATUMBLANK": datum triangle
   * - "_INTEGRAL": integral
   * - "_ARCHTICK": architectural tick
   */
  DIMBLK?: string

  /**
   * The arrowhead for the first end of the dimension line when DIMSAH is on.
   * Initial value:	""
   */
  DIMBLK1?: string

  /**
   * The arrowhead for the second end of the dimension line when DIMSAH is on.
   * Initial value:	""
   */
  DIMBLK2?: string

  /**
   * Controls drawing of circle or arc center marks and centerlines by the DIMCENTER, DIMDIAMETER, and DIMRADIUS commands.
   * Initial value:	0.0900 (imperial) or 2.5000 (metric)
   * - 0: No center marks or lines are drawn
   * - <0: Centerlines are drawn
   * - >0: Center marks are drawn
   */
  DIMCEN?: number

  /**
   * Dimension line color: range is 0 = BYBLOCK; 256 = BYLAYER
   * Initial value:	0
   */
  DIMCLRD?: number

  /**
   * Dimension extension line color
   * - 0 = BYBLOCK
   * - 256 = BYLAYER
   */
  DIMCLRE?: number

  /**
   * Dimension text color
   * - 0 = BYBLOCK
   * - 256 = BYLAYER
   */
  DIMCLRT?: number

  /**
   * Number of decimal places for the tolerance values of a primary units dimension
   */
  DIMDEC?: number

  /**
   * Dimension line extension
   */
  DIMDLE?: number

  /**
   * Dimension line increment
   */
  DIMDLI?: number

  /**
   * Single-character decimal separator used when creating dimensions whose unit format is decimal
   */
  DIMDSEP?: string

  /**
   * Extension line extension
   */
  DIMEXE?: number

  /**
   * Extension line offset
   */
  DIMEXO?: number

  /**
   * Scale factor used to calculate the height of text for dimension fractions and tolerances
   */
  DIMFAC?: number

  /**
   * Dimension line gap
   */
  DIMGAP?: number

  /**
   * Horizontal dimension text position
   * - 0 = Above dimension line and center-justified between extension lines
   * - 1 = Above dimension line and next to first extension line
   * - 2 = Above dimension line and next to second extension line
   * - 3 = Above and center-justified to first extension line
   * - 4 = Above and center-justified to second extension line
   */
  DIMJUST?: number

  /**
   * Arrow block name for leaders
   */
  DIMLDRBLK?: string

  /**
   * Linear measurements scale factor
   */
  DIMLFAC?: number

  /**
   * Dimension limits generated if nonzero
   */
  DIMLIM?: number

  /**
   * Sets units for all dimension types except Angular
   * - 1 = Scientific
   * - 2 = Decimal
   * - 3 = Engineering
   * - 4 = Architectural
   * - 5 = Fractional
   * - 6 = Operating system
   */
  DIMLUNIT?: number

  /**
   * Dimension line lineweight
   * - -3 = Standard
   * - -2 = ByLayer
   * - -1 = ByBlock
   * - 0-211 = 100th of mm
   */
  DIMLWD?: number

  /**
   * Extension line lineweight
   * - -3 = Standard
   * - -2 = ByLayer
   * - -1 = ByBlock
   * - 0-211 = 100th of mm
   */
  DIMLWE?: number

  /**
   * General dimensioning suffix
   */
  DIMPOST?: string

  /**
   * Rounding value for dimension distances
   */
  DIMRND?: number

  /**
   * Use separate arrow blocks if nonzero
   */
  DIMSAH?: number

  /**
   * Overall dimensioning scale factor
   */
  DIMSCALE?: number

  /**
   * Suppression of first extension line
   * - 0 = Not suppressed
   * - 1 = Suppressed
   */
  DIMSD1?: number

  /**
   * Suppression of second extension line
   * - 0 = Not suppressed
   * - 1 = Suppressed
   */
  DIMSD2?: number

  /**
   * First extension line suppressed if nonzero
   */
  DIMSE1?: number

  /**
   * Second extension line suppressed if nonzero
   */
  DIMSE2?: number

  /**
   * Recompute dimensions while dragging
   * - 0 = Drag original image
   * - 1 = Recompute
   */
  DIMSHO?: number

  /**
   * Suppress outside-extensions dimension lines if nonzero
   */
  DIMSOXD?: number

  /**
   * Dimension style name
   */
  DIMSTYLE?: string

  /**
   * Text above dimension line if nonzero
   */
  DIMTAD?: number

  /**
   * Number of decimal places to display the tolerance values
   */
  DIMTDEC?: number

  /**
   * Dimension tolerance display scale factor
   */
  DIMTFAC?: number

  /**
   * Text inside horizontal if nonzero
   */
  DIMTIH?: number

  /**
   * Force text inside extensions if nonzero
   */
  DIMTIX?: number

  /**
   * Minus tolerance
   */
  DIMTM?: number

  /**
   * Dimension text movement rules
   * - 0 = Moves the dimension line with dimension text
   * - 1 = Adds a leader when dimension text is moved
   * - 2 = Allows text to be moved freely without a leader
   */
  DIMTMOVE?: number

  /**
   * If text is outside the extension lines, dimension lines are forced between the extension lines if nonzero
   */
  DIMTOFL?: number

  /**
   * Text outside horizontal if nonzero
   */
  DIMTOH?: number

  /**
   * Dimension tolerances generated if nonzero
   */
  DIMTOL?: number

  /**
   * Vertical justification for tolerance values
   * - 0 = Top
   * - 1 = Middle
   * - 2 = Bottom
   */
  DIMTOLJ?: number

  /**
   * Plus tolerance
   */
  DIMTP?: number

  /**
   * Dimensioning tick size
   * - 0 = Draws arrowheads
   * - >0 = Draws oblique strokes
   */
  DIMTSZ?: number

  /**
   * Text vertical position
   */
  DIMTVP?: number

  /**
   * Dimension text style
   */
  DIMTXSTY?: string

  /**
   * Dimensioning text height
   */
  DIMTXT?: number

  /**
   * Controls suppression of zeros for tolerance values
   * - 0 = Suppresses zero feet and precisely zero inches
   * - 1 = Includes zero feet and precisely zero inches
   * - 2 = Includes zero feet and suppresses zero inches
   * - 3 = Includes zero inches and suppresses zero feet
   * - 4 = Suppresses leading zeros in decimal dimensions
   * - 8 = Suppresses trailing zeros in decimal dimensions
   * - 12 = Suppresses both leading and trailing zeros
   */
  DIMTZIN?: number

  /**
   * Cursor functionality for user-positioned text
   * - 0 = Controls only the dimension line location
   * - 1 = Controls the text position as well
   */
  DIMUPT?: number

  /**
   * Controls suppression of zeros for primary unit values
   * - 0 = Suppresses zero feet and precisely zero inches
   * - 1 = Includes zero feet and precisely zero inches
   * - 2 = Includes zero feet and suppresses zero inches
   * - 3 = Includes zero inches and suppresses zero feet
   * - 4 = Suppresses leading zeros in decimal dimensions
   * - 8 = Suppresses trailing zeros in decimal dimensions
   * - 12 = Suppresses both leading and trailing zeros
   */
  DIMZIN?: number

  /**
   * Controls the display of silhouette curves of body objects in Wireframe mode
   * - 0 = Off
   * - 1 = On
   */
  DISPSILH?: number

  /**
   * Hard-pointer ID to visual style while creating 3D solid primitives. The default value is NULL
   */
  DRAGVS?: string | null

  /**
   * Drawing code page; set to the system code page when a new drawing is created
   */
  DWGCODEPAGE?: string

  /**
   * Current elevation set by ELEV command
   */
  ELEVATION?: number

  /**
   * Lineweight endcaps setting for new objects
   * - 0 = None
   * - 1 = Round
   * - 2 = Angle
   * - 3 = Square
   */
  ENDCAPS?: number

  /**
   * X, Y, and Z drawing extents upper-right corner (in WCS)
   */
  EXTMAX?: DwgPoint3D

  /**
   * X, Y, and Z drawing extents lower-left corner (in WCS)
   */
  EXTMIN?: DwgPoint3D

  /**
   * Controls symbol table naming
   * - 0 = AutoCAD Release 14 compatibility
   * - 1 = AutoCAD 2000
   */
  EXTNAMES?: number

  /**
   * Fillet radius
   */
  FILLETRAD?: number

  /**
   * Fill mode on if nonzero
   */
  FILLMODE?: number

  /**
   * Set at creation time, uniquely identifies a particular drawing
   */
  FINGERPRINTGUID?: string

  /**
   * Specifies a gap to be displayed where an object is hidden by another object
   */
  HALOGAP?: number

  /**
   * Next available handle
   */
  HANDSEED?: string

  /**
   * Specifies HIDETEXT system variable
   * - 0 = HIDE ignores text objects
   * - 1 = HIDE does not ignore text objects
   */
  HIDETEXT?: number

  /**
   * Path for all relative hyperlinks in the drawing
   */
  HYPERLINKBASE?: string

  /**
   * Controls whether layer and spatial indexes are created and saved in drawing files
   * - 0 = No indexes are created
   * - 1 = Layer index is created
   * - 2 = Spatial index is created
   * - 3 = Layer and spatial indexes are created
   */
  INDEXCTL?: number

  /**
   * Insertion base set by BASE command (in WCS)
   */
  INSBASE?: string

  /**
   * Default drawing units for AutoCAD DesignCenter blocks
   * - 0: Unitless
   * - 1: Inches
   * - 2: Feet
   * - 3: Miles
   * - 4: Millimeters
   * - 5: Centimeters
   * - 6: Meters
   * - 7: Kilometers
   * - 8: Microinches
   * - 9: Mils
   * - 10: Yards
   * - 11: Angstroms
   * - 12: Nanometers
   * - 13: Microns
   * - 14: Decimeters
   * - 15: Decameters
   * - 16: Hectometers
   * - 17: Gigameters
   * - 18: Astronomical units
   * - 19: Light years
   * - 20: Parsecs
   * - 21: US Survey Feet
   * - 22: US Survey Inch
   * - 23: US Survey Yard
   * - 24: US Survey Mile
   */
  INSUNITS?: number

  /**
   * Represents the ACI color index of the "interference objects" created during the INTERFERE command
   * Default value is 1
   */
  INTERFERECOLOR?: number

  /**
   * Hard-pointer ID to the visual style for interference objects
   * Default visual style is Conceptual
   */
  INTERFEREOBJVS?: string

  /**
   * Hard-pointer ID to the visual style for the viewport during interference checking
   * Default visual style is 3d Wireframe
   */
  INTERFEREVPVS?: string

  /**
   * Specifies the entity color of intersection polylines
   * Values 1-255 designate an AutoCAD color index (ACI)
   * 0 = Color BYBLOCK
   * 256 = Color BYLAYER
   * 257 = Color BYENTITY
   */
  INTERSECTIONCOLOR?: number

  /**
   * Specifies the display of intersection polylines
   * - 0: Turns off the display of intersection polylines
   * - 1: Turns on the display of intersection polylines
   */
  INTERSECTIONDISPLAY?: number

  /**
   * Lineweight joint setting for new objects
   * - 0: None
   * - 1: Round
   * - 2: Angle
   * - 3: Flat
   */
  JOINSTYLE?: number

  /**
   * Nonzero if limits checking is on
   */
  LIMCHECK?: number

  /**
   * XY drawing limits upper-right corner (in WCS)
   */
  LIMMAX?: string

  /**
   * XY drawing limits lower-left corner (in WCS)
   */
  LIMMIN?: string

  /**
   * Global linetype scale
   */
  LTSCALE?: number

  /**
   * Units format for coordinates and distances
   */
  LUNITS?: number

  /**
   * Units precision for coordinates and distances
   */
  LUPREC?: number

  /**
   * Controls the display of lineweights on the Model or Layout tab
   * - 0: Lineweight is not displayed
   * - 1: Lineweight is displayed
   */
  LWDISPLAY?: number

  /**
   * Sets maximum number of viewports to be regenerated
   */
  MAXACTVP?: number

  /**
   * Sets drawing units
   * - 0: English
   * - 1: Metric
   */
  MEASUREMENT?: number

  /**
   * Name of menu file
   */
  MENU?: string

  /**
   * Mirror text if nonzero
   */
  MIRRTEXT?: number

  /**
   * Specifies the color of obscured lines
   * - 0 and 256: Entity color
   * - 1-255: An AutoCAD color index (ACI)
   */
  OBSCOLOR?: number

  /**
   * Specifies the linetype of obscured lines
   * - 0: Off
   * - 1: Solid
   * - 2: Dashed
   * - 3: Dotted
   * - 4: Short Dash
   * - 5: Medium Dash
   * - 6: Long Dash
   * - 7: Double Short Dash
   * - 8: Double Medium Dash
   * - 9: Double Long Dash
   * - 10: Medium Long Dash
   * - 11: Sparse Dot
   */
  OBSLTYPE?: number

  /**
   * Ortho mode on if nonzero
   */
  ORTHOMODE?: number

  /**
   * Point display mode
   */
  PDMODE?: number

  /**
   * Point display size
   */
  PDSIZE?: number

  /**
   * Current paper space elevation
   */
  PELEVATION?: number

  /**
   * Maximum X, Y, and Z extents for paper space
   */
  PEXTMAX?: string

  /**
   * Minimum X, Y, and Z extents for paper space
   */
  PEXTMIN?: string

  /**
   * Paper space insertion base point
   */
  PINSBASE?: string

  /**
   * Limits checking in paper space when nonzero
   */
  PLIMCHECK?: number

  /**
   * Maximum X and Y limits in paper space
   */
  PLIMMAX?: string

  /**
   * Minimum X and Y limits in paper space
   */
  PLIMMIN?: string

  /**
   * Governs the generation of linetype patterns around the vertices of a 2D polyline
   * - 1: Linetype is generated in a continuous pattern around vertices of the polyline
   * - 0: Each segment of the polyline starts and ends with a dash
   */
  PLINEGEN?: number

  /**
   * Default polyline width
   */
  PLINEWID?: number

  /**
   * Assigns a project name to the current drawing
   */
  PROJECTNAME?: string

  /**
   * Controls the saving of proxy object images
   */
  PROXYGRAPHICS?: number

  /**
   * Controls paper space linetype scaling
   * - 1: No special linetype scaling
   * - 0: Viewport scaling governs linetype scaling
   */
  PSLTSCALE?: number

  /**
   * Indicates whether the current drawing is in a Color-Dependent or Named Plot Style mode
   * - 0: Uses named plot style tables in the current drawing
   * - 1: Uses color-dependent plot style tables in the current drawing
   */
  PSTYLEMODE?: number

  /**
   * View scale factor for new viewports
   * - 0: Scaled to fit
   * - >0: Scale factor (a positive real value)
   */
  PSVPSCALE?: number

  /**
   * Name of the UCS that defines the origin and orientation of orthographic UCS settings (paper space only)
   */
  PUCSBASE?: string

  /**
   * Current paper space UCS name
   */
  PUCSNAME?: string

  /**
   * Current paper space UCS origin
   */
  PUCSORG?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to BACK when PUCSBASE is set to WORLD
   */
  PUCSORGBACK?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to BOTTOM when PUCSBASE is set to WORLD
   */
  PUCSORGBOTTOM?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to FRONT when PUCSBASE is set to WORLD
   */
  PUCSORGFRONT?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to LEFT when PUCSBASE is set to WORLD
   */
  PUCSORGLEFT?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to RIGHT when PUCSBASE is set to WORLD
   */
  PUCSORGRIGHT?: string

  /**
   * Point which becomes the new UCS origin after changing paper space UCS to TOP when PUCSBASE is set to WORLD
   */
  PUCSORGTOP?: string

  /**
   * If paper space UCS is orthographic (PUCSORTHOVIEW not equal to 0), this is the name of the UCS that the orthographic UCS is relative to
   */
  PUCSORTHOREF?: string

  /**
   * Orthographic view type of paper space UCS
   * - 0: UCS is not orthographic
   * - 1: Top
   * - 2: Bottom
   * - 3: Front
   * - 4: Back
   * - 5: Left
   * - 6: Right
   */
  PUCSORTHOVIEW?: number

  /**
   * Current paper space UCS X axis
   */
  PUCSXDIR?: number

  /**
   * Current paper space UCS Y axis
   */
  PUCSYDIR?: number

  /**
   * Quick Text mode on if nonzero
   */
  QTEXTMODE?: number

  /**
   * REGENAUTO mode on if nonzero
   */
  REGENMODE?: number

  /**
   * Controls the shading of edges
   * - 0: Faces shaded, edges not highlighted
   * - 1: Faces shaded, edges highlighted in black
   * - 2: Faces not filled, edges in entity color
   * - 3: Faces in entity color, edges in black
   */
  SHADEDGE?: number

  /**
   * Percent ambient/diffuse light; range 1-100; default 70
   */
  SHADEDIF?: number

  /**
   * Location of the ground shadow plane. This is a Z axis ordinate.
   */
  SHADOWPLANELOCATION?: number

  /**
   * Sketch record increment
   */
  SKETCHINC?: number

  /**
   * Determines the object type created by the SKETCH command
   * - 0: Generates lines
   * - 1: Generates polylines
   * - 2: Generates splines
   */
  SKPOLY?: number

  /**
   * Controls the object sorting methods; accessible from the Options dialog box User Preferences tab. SORTENTS uses the following bitcodes
   * - 0: Disables SORTENTS
   * - 1: Sorts for object selection
   * - 2: Sorts for object snap
   * - 4: Sorts for redraws; obsolete
   * - 8: Sorts for MSLIDE command slide creation; obsolete
   * - 16: Sorts for REGEN commands
   * - 32: Sorts for plotting
   * - 64: Sorts for PostScript output; obsolete
   */
  SORTENTS?: number

  /**
   * Number of line segments per spline patch
   */
  SPLINESEGS?: number

  /**
   * Spline curve type for PEDIT Spline
   */
  SPLINETYPE?: number

  /**
   * Number of mesh tabulations in first direction
   */
  SURFTAB1?: number

  /**
   * Number of mesh tabulations in second direction
   */
  SURFTAB2?: number

  /**
   * Surface type for PEDIT Smooth
   */
  SURFTYPE?: number

  /**
   * Surface density (for PEDIT Smooth) in M direction
   */
  SURFU?: number

  /**
   * Surface density (for PEDIT Smooth) in N direction
   */
  SURFV?: number

  /**
   * Local date/time of drawing creation
   */
  TDCREATE?: Date

  /**
   * Cumulative editing time for this drawing
   */
  TDINDWG?: number

  /**
   * Universal date/time the drawing was created
   */
  TDUCREATE?: Date

  /**
   * Local date/time of last drawing update
   */
  TDUPDATE?: Date

  /**
   * User-elapsed timer
   */
  TDUSRTIMER?: number

  /**
   * Universal date/time of the last update/save
   */
  TDUUPDATE?: Date

  /**
   * Default text height
   */
  TEXTSIZE?: number

  /**
   * Current text style name
   */
  TEXTSTYLE?: string

  /**
   * Current thickness set by ELEV command
   */
  THICKNESS?: number

  /**
   * 1 for previous release compatibility mode; 0 otherwise
   */
  TILEMODE?: number

  /**
   * Default trace width
   */
  TRACEWID?: number

  /**
   * Specifies the maximum depth of the spatial index
   */
  TREEDEPTH?: number

  /**
   * Name of the UCS that defines the origin and orientation of orthographic UCS settings
   */
  UCSBASE?: string

  /**
   * Name of current UCS
   */
  UCSNAME?: string

  /**
   * Origin of current UCS (in WCS)
   */
  UCSORG?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to BACK when UCSBASE is set to WORLD
   */
  UCSORGBACK?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to BOTTOM when UCSBASE is set to WORLD
   */
  UCSORGBOTTOM?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to FRONT when UCSBASE is set to WORLD
   */
  UCSORGFRONT?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to LEFT when UCSBASE is set to WORLD
   */
  UCSORGLEFT?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to RIGHT when UCSBASE is set to WORLD
   */
  UCSORGRIGHT?: DwgPoint3D

  /**
   * Point which becomes the new UCS origin after changing model space UCS to TOP when UCSBASE is set to WORLD
   */
  UCSORGTOP?: DwgPoint3D

  /**
   * If model space UCS is orthographic (UCSORTHOVIEW not equal to 0), this is the name of the UCS that the orthographic UCS is relative to. If blank, UCS is relative to WORLD
   */
  UCSORTHOREF?: string

  /**
   * Orthographic view type of model space UCS
   * - 0: UCS is not orthographic
   * - 1: Top
   * - 2: Bottom
   * - 3: Front
   * - 4: Back
   * - 5: Left
   * - 6: Right
   */
  UCSORTHOVIEW?: number

  /**
   * Direction of the current UCS X axis (in WCS)
   */
  UCSXDIR?: number

  /**
   * Direction of the current UCS Y axis (in WCS)
   */
  UCSYDIR?: number

  /**
   * Low bit set = Display fractions, feet-and-inches, and surveyor's angles in input format
   */
  UNITMODE?: number

  /**
   * Five integer variables intended for use by third-party developers
   */
  USERI1?: number
  USERI2?: number
  USERI3?: number
  USERI4?: number
  USERI5?: number

  /**
   * Five real variables intended for use by third-party developers
   */
  USERR1?: number
  USERR2?: number
  USERR3?: number
  USERR4?: number
  USERR5?: number

  /**
   * Controls the user timer for the drawing
   * - 0: Timer off
   * - 1: Timer on
   */
  USRTIMER?: number

  /**
   * Uniquely identifies a particular version of a drawing. Updated when the drawing is modified
   */
  VERSIONGUID?: string

  /**
   * Controls the properties of xref-dependent layers
   * - 0: Don't retain xref-dependent visibility settings
   * - 1: Retain xref-dependent visibility settings
   */
  VISRETAIN?: number

  /**
   * Determines whether input for the DVIEW and VPOINT command evaluated as relative to the WCS or current UCS
   * - 0: Don't change UCS
   * - 1: Set UCS to WCS during DVIEW/VPOINT
   */
  WORLDVIEW?: number

  /**
   * Controls the visibility of xref clipping boundaries
   * - 0: Clipping boundary is not visible
   * - 1: Clipping boundary is visible
   */
  XCLIPFRAME?: number

  /**
   * Controls whether the current drawing can be edited in-place when being referenced by another drawing
   * - 0: Can't use in-place reference editing
   * - 1: Can use in-place reference editing
   */
  XEDIT?: number
}
