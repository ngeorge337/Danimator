#pragma once

enum
{
	END_NONE = 0,
	END_LOOP,
	END_WAIT,
	END_STOP,
	END_GOTO,
};

// A single frame of animation
struct Frame_t
{
	//frame_t() : duration(1.f), sprite(nullptr), action([] () {})
	Frame_t();;

	float duration;
	int tics;
	sf::Sprite sprite;
	std::string spriteName;
	std::string soundName;
	bool hasSound;
};

// A series of frames creating a state of animation
struct FState_t
{
	FState_t() : m_ticks(0.f), m_overrideColor(sf::Color::White), _scaleX(1.f), _scaleY(1.f), flippedX(false), flippedY(false),
		m_frames(std::vector<Frame_t>()), name(""), frameOffset(0), ending(END_LOOP), gotoLabel(""), justEntered(true)
	{
		m_overrideColor = sf::Color::White;
	}

	BYTE ending;
	std::string gotoLabel;
	std::string name;
	std::vector<Frame_t> m_frames;

	float m_ticks;
	sf::Color m_overrideColor;
	float _scaleX, _scaleY;
	bool flippedX, flippedY;
	bool justEntered;

	sf::Vector2f m_posOffset;

	int frameOffset;

	void Reset(int offset = 0);
	Frame_t &AddFrame(std::shared_ptr<sf::Texture> _texture, float _duration);
	Frame_t &AddEmptyFrame();
	Frame_t &InsertEmptyFrame(int pos = -1);
	void RemoveFrame(int pos = -1);
	Frame_t &GetCurrentFrame();
	Frame_t &GetFrame(int offset);
	bool Tick(float t);
	int GetOffset();
	void SetOffset(int f);

	void SetScale(float _x, float _y);
	const sf::Vector2f GetScale();
	void FlipX(bool flip);
	void FlipY(bool flip);
	void SetColor(sf::Color clr);
};

// Controls the states and changes between them as well as some control properties
class Animator : public sf::Drawable
{
	friend class DanFrame;
	friend class DanCode;
	friend class ActionManager;
	friend class DanStateList;
public:
	Animator();

	FState_t *CreateState(const std::string &_name);
	void DeleteState(const std::string &_name);
	void DuplicateState(const std::string &name, const std::string &newName);
	void RenameState(const std::string &name, const std::string &newName);
	FState_t *GetState(const std::string &_name);
	FState_t *GetCurrentState();
	void SetNull();

	bool IsValidState(const std::string &_name);

	void ClearStates();
	void ResetStates();
	void SetState(const std::string &_name, int offset = 0);
	void Tick(float t);

	void SetStateFlipX(bool x);
	void SetStateFlipY(bool y);

	void EnsureSpritePointers();

	void UpdateSpriteNames(const std::string &oldname, const std::string &newname);
	void UpdateSoundNames(const std::string &oldname, const std::string &newname);


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	std::map<std::string, FState_t> m_validStates;
	FState_t *m_currentState;
};
