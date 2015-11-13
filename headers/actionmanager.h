#pragma once

// What needs to be stored for an undo/redo?
// - Animation States
// - Animation Frames (including offsets etc but that is included as part of the frame's state)
// - Loaded Texture Names
// - Loaded Sound Names
// These will have to be tracked whenever an action is taken
// Likewise, things like timeline information and such has to update when an action is undone/redone
// luckily this is easy enough with all the Update-related commands; just call into them when
// something is modified as usual

// Other than the texture pointer stored in sf::Sprite, frame_t does not store any pointers
// This makes it easy to just store a complete copy of the frame information into the stack

//struct FState_t;

enum
{
	CMDTYPE_FRAMES = 1,		// Frames of animation were modified (add, removed, offsets, sprite, sound, etc.), or State was modified (deleted, added, renamed)
	CMDTYPE_TEXTURELOAD,	// Textures were loaded in
	CMDTYPE_SOUNDLOAD,		// Sounds were loaded in
};

struct action_t
{
	int actionType;
	std::string actMessage;
	std::string lastState;
	std::map<std::string, FState_t> oldState;
	std::vector<rowData_t> oldStateList;
	std::vector<wxListItem> oldSpriteList;
	std::vector<wxListItem> oldSoundList;
};

// Yeah this is a dumb mess but "yeah fuckin' whatever"
class ActionManager
{
public:
	void Insert(int type, const std::string &actionMessage);
	void Undo();
	void Redo();
	void Wipe();
private:
	void InsertRedo(int type, const std::string &actionMessage);
	void InsertUndo(int type, const std::string &actionMessage);
	void Finish();

	std::deque<action_t> m_commands;
	std::deque<action_t> m_undone;
};

