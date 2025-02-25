/* Copyright (C) 2021 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
GUI Object - Drop Down (list)

--Overview--

	Works just like a list-box, but it hides
	all the elements that aren't selected. They
	can be brought up by pressing the control.
*/

#ifndef INCLUDED_CDROPDOWN
#define INCLUDED_CDROPDOWN

#include "gui/CGUISprite.h"
#include "gui/ObjectBases/IGUIObject.h"
#include "gui/ObjectTypes/CList.h"
#include "maths/Vector2D.h"

#include <string>

/**
 * Drop Down
 *
 * The control can be pressed, but we will not inherent
 * this behavior from IGUIButtonBehavior, because when
 * you press this control, the list with elements will
 * immediately appear, and not first after release
 * (which is the whole gist of the IGUIButtonBehavior).
 */
class CDropDown : public CList
{
	GUI_OBJECT(CDropDown)

public:
	CDropDown(CGUI& pGUI);
	virtual ~CDropDown();

	/**
	 * @see IGUIObject#HandleMessage()
	 */
	virtual void HandleMessage(SGUIMessage& Message);

	/**
	 * Handle events manually to catch keyboard inputting.
	 */
	virtual InReaction ManuallyHandleKeys(const SDL_Event_* ev);

	/**
	 * Draws the Button
	 */
	virtual void Draw(CCanvas2D& canvas);

	// This is one of the few classes we actually need to redefine this function
	//  this is because the size of the control changes whether it is open
	//  or closed.
	virtual bool IsMouseOver() const;

	virtual float GetBufferedZ() const;

protected:
	/**
	 * If the size changed, the texts have to be updated as
	 * the word wrapping depends on the size.
	 */
	virtual void UpdateCachedSize();

	/**
	 * Sets up text, should be called every time changes has been
	 * made that can change the visual.
	 */
	void SetupText();

	// Sets up the cached GetListRect. Decided whether it should
	//  have a scrollbar, and so on.
	virtual void SetupListRect();

	// Specify a new List rectangle.
	virtual CRect GetListRect() const;

	/**
	 * Placement of text.
	 */
	CVector2D m_TextPos;

	// Is the dropdown opened?
	bool m_Open;

	// I didn't cache this at first, but it's just as easy as caching
	//  m_CachedActualSize, so I thought, what the heck it's used a lot.
	CRect m_CachedListRect;

	// Hide scrollbar when it's not needed
	bool m_HideScrollBar;

	// Not necessarily the element that is selected, this is just
	//  which element should be highlighted. When opening the dropdown
	//  it is set to "selected", but then when moving the mouse it will
	//  change.
	int m_ElementHighlight;

	// Stores any text entered by the user for quick access to an element
	// (ie if you type "acro" it will take you to acropolis).
	std::string m_InputBuffer;

	// used to know if we want to restart anew or add to m_inputbuffer.
	double m_TimeOfLastInput;

	// Settings
	CGUISimpleSetting<float> m_ButtonWidth;
	CGUISimpleSetting<float> m_DropDownSize;
	CGUISimpleSetting<float> m_DropDownBuffer;
	CGUISimpleSetting<u32> m_MinimumVisibleItems;
	CGUISimpleSetting<CStrW> m_SoundClosed;
	CGUISimpleSetting<CStrW> m_SoundEnter;
	CGUISimpleSetting<CStrW> m_SoundLeave;
	CGUISimpleSetting<CStrW> m_SoundOpened;
	CGUISimpleSetting<CGUISpriteInstance> m_SpriteDisabled;
	CGUISimpleSetting<CGUISpriteInstance> m_SpriteOverlayDisabled;
	CGUISimpleSetting<CGUISpriteInstance> m_SpriteList;
	CGUISimpleSetting<CGUISpriteInstance> m_SpriteListOverlay;
	CGUISimpleSetting<CGUISpriteInstance> m_Sprite2;
	CGUISimpleSetting<CGUISpriteInstance> m_Sprite2Over;
	CGUISimpleSetting<CGUISpriteInstance> m_Sprite2Pressed;
	CGUISimpleSetting<CGUISpriteInstance> m_Sprite2Disabled;
	CGUISimpleSetting<CGUIColor> m_TextColorDisabled;
};

#endif // INCLUDED_CDROPDOWN
