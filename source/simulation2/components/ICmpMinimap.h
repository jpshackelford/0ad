/* Copyright (C) 2022 Wildfire Games.
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

#ifndef INCLUDED_ICMPMINIMAP
#define INCLUDED_ICMPMINIMAP

#include "simulation2/system/Interface.h"

#include "simulation2/helpers/Position.h"

#include <string>

/**
 * Per-unit minimap data.
 */
class ICmpMinimap : public IComponent
{
public:
	/**
	 * Gets the data for rendering this entity on the minimap.
	 * If it should not be drawn, returns false; otherwise the arguments are set
	 * to the color and world position.
	 */
	virtual bool GetRenderData(u8& r, u8& g, u8& b, entity_pos_t& x, entity_pos_t& z) const = 0;

	/**
	 * Returns true if the entity is actively pinging based on the current time.
	 */
	virtual bool CheckPing(double currentTime, double pingDuration) = 0;

	/**
	 * Updates the entity's minimap color to match the player color.
	 */
	virtual void UpdateColor() = 0;

	/**
	 * Returns true if a minimap should have icon of this entity.
	 */
	virtual bool HasIcon() = 0;

	/**
	 * Returns a path to icon of this entity.
	 */
	virtual std::string GetIconPath() = 0;

	virtual float GetIconSize() = 0;

	DECLARE_INTERFACE_TYPE(Minimap)
};

#endif // INCLUDED_ICMPMINIMAP
