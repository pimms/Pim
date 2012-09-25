#pragma once

namespace Pim
{
	/*
		A colliding object has two values related to filtering collisions. 
		A game node has one bitfield containing the group/groups to which it belong,
		and a bitfield containing which groups it can collide with.

		For instance, the player can belong to group 1, and only be able to collide
		with things from group 2, 4 and 8. If an NPC or tree belongs to group 1, no
		collision will be triggered at all.
	*/
	typedef enum CollisionFilter
	{
		FILTER_1 = 1,
		FILTER_2 = 2,
		FILTER_3 = 4,
		FILTER_4 = 8, 

		FILTER_MAX = 4,		// You can set this manually if you're building from source.
	};
}