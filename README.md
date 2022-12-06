This is the Mowitz ("More widgets") library.

The project's goal is to create a library of widgets for X applications
to use. The widgets have been snarfed from various sources and are
all open source (GPL or MIT licenses).


Available widgets
-----------------

 - Animator
 - Canvas
 - Check
 - Combo
 - Frame
 - Handle
 - Html
 - Image
 - ListTree
 - Notebook
 - Richtext
 - Rudegrid
 - Ruler
 - Slider
 - Spinner
 - Tabbing
 - Table
 - Tabs
 - TextField
 - Tooltip
 - VSlider

And a complete menu kit.

The original reason for creating this library is that the Athena
widget set lacks a lot of widgets that would be useful, and many
of the existing widgets leave a lot to be desired in appearance
and behaviour.

Once upon a time there was a project called the Free Widget Foundation.
It started out with goals similar to those of Mowitz, but later
turned into a project to create a complete, standalone widget set.
That is not something that will happen here.


Namespace
---------

All external symbols are prefixed with "Mw" in order to avoid
name space clashes with other libraries. So for example, the
spinner widget is called MwSpinner.


Applications using Mowitz
-------------------------

Since most of these widgets were previously distributed with
Siag Office, it is natural that the upcoming release of that
package will use (and require) the Mowitz library.

Many existing X applications can be trivially updated to use
the widgets in the Mowitz library.


Utility routines
----------------

In addition to the widgets themselves, Mowitz includes several
utility functions that are used within the library, but useful
to application writers as well. The functionality includes:

 - Loading and caching pixmaps
 - Allocating and caching pixel values



Mowitz à la carte
-----------------

It is possible to pick and choose among the widgets within Mowitz.
Normally there is no reason to do so, but it can be a way to add
a single widget to an application without having to depend on
another library. It can also be a way to avoid licensing issues.

To do this, copy the MwUtils.c and MwUtils.h files to the application,
plus the files for the widget. All widgets use the routines in
MwUtils, so it must always be included.

Some widgets are subclassed from other widgets in the library. The
Handle widget, for example, is subclassed from Frame. In that case,
both widgets must be included.


Widget hierarchy
----------------

Names in brackets are Intrinsic widgets, not part of Mowitz.

[RectObj]
  |-BaseME
  |  |-LabelME
  |  |  |-CheckME
  |  |  |-MBButton
  |  |  `-SubME
  |  `-LineME
[Core]
  |-Base
  |  `-SButton
  |     `-Button
  |        `-MenuButton
  |-Canvas
  |-Check
  |-[Composite]
  |   |-Animator
  |   |-BaseComp
  |   |-[Constraint]
  |   |   |-BaseConst
  |   |   |   |-Row
  |   |   |   `-MenuBar
  |   |   |-Rudegrid
  |   |   |  `-Filesel
  |   |   `-Tabs
  |   |-Frame
  |   |  |-Combo
  |   |  |-Handle
  |   |  `-Spinner
  |   |-Richtext
  |   |-[Shell]
  |   |   `-[OverrideShell]
  |   |       |-Menu
  |   |       |-PopText
  |   |       `-Tooltip
  |   `-Table
  |-Image
  |-ListTree
  |-Notebook
  |-Ruler
  |-Slider
  |  `-VSlider
  |-Tabstop
  `-TextField


Authors
-------

Ondrejicka Stefan: Menus
Edward A. Falk: Frame, Ruler, Slider, VSlider, Tabs
Robert W. McMullen: ListTree, TextField
Ulric Eriksson: Animator, Canvas, Check, Combo, Filesel,
		Handle, Image, Notebook, Richtext,
		Rudegrid, Spinner, Tabbing, Table, Tabs, Tabstop, Tooltip

