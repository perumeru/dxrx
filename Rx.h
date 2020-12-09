#pragma once
#include "DxLib.h"
#include <cstddef>
#include <vector>
#include <memory>

#ifdef _MSC_VER
#include <tchar.h>
#else
#ifdef _UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif
#endif
#include <string>
#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
#if defined(_DEBUG) && !defined(NEW)
#define NEW  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW  new(std::nothrow)
#endif
// wchar_t サロゲートペア判定( UTF-32 or UTF-16 想定 )
#ifdef WCHAR_T_BE
#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( static_cast<WORD>( x ) & 0x00fc ) == 0x00d8 ) )
#else
#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( static_cast<WORD>( x ) & 0xfc00 ) == 0xd800 ) )
#endif
#define SAFEDELETE(ptr)if(ptr){delete ptr;ptr=nullptr;}
#define getset(type,value,name)\
void set##name(type value) {\
if(m_##value != value)\
m_##value = value;\
}\
inline type get##name() {\
	return m_##value;\
}
#define getset_s(type,value,name)\
void set##name(type value) {\
m_##value = value;\
}\
inline type get##name() {\
	return m_##value;\
}
#define s_(d,v)static_cast<d>(v)
#define d_(d,v)dynamic_cast<d>(v)
#define r_(d,v)reinterpret_cast<d>(v)
namespace MemoryPool {
	template <class T, std::size_t Size>
	class MemoryFirstinsure
	{
	public:
		inline MemoryFirstinsure() :itr(0) {
			pool.resize(Size + 1);
			for (int i = 0; i < Size + 1; i++)
				pool[i] = static_cast<T*>(malloc(sizeof(T)));
		}
		inline ~MemoryFirstinsure() {
			for (T * itr : pool) {
				free(itr);
			}
		}
		void poolMarze() {
			size_t s = itr;
			pool.resize(s + Size + 1);
			for (unsigned i = s; i < s + Size + 1; i++)
				pool[i] = static_cast<T*>(malloc(sizeof(T)));
		}
		inline T* Get() {
			return pool[itr++];
		}
	private:
		std::vector<T*> pool;
		unsigned int itr;
	};

	template <class T, std::size_t Size>
	class MemoryReuse final {
		using selfMemoryPtr = MemoryReuse<T, Size>*;
	public:
		inline MemoryReuse() {
			pool.reset(new MemoryFirstinsure<T, Size>);
			expandFreemem();
		}
		inline ~MemoryReuse() = default;
		inline void* alloc() {
			if (!next) {
				pool->poolMarze();
				expandFreemem();
			}
			selfMemoryPtr head = next;
			next = head->next;
			return head;
		}

		inline void nextmove(void* freePtr) {
			selfMemoryPtr head = static_cast<selfMemoryPtr>(freePtr);
			head->next = next;
			next = head;
		}
	private:
		inline void expandFreemem() {
			selfMemoryPtr running = reinterpret_cast<selfMemoryPtr>(pool->Get());
			next = running;
			for (int i = 0; i < Size; ++i) {
				running->next = reinterpret_cast<selfMemoryPtr>(pool->Get());
				running = running->next;
			}
			running->next = nullptr;
		}
	private:
		std::unique_ptr<MemoryFirstinsure<T, Size>> pool;
		selfMemoryPtr next;
	};
	template <class T, std::size_t Size = 256>
	class Forbullets_pool final
	{
	public:
		inline Forbullets_pool() = default;
		inline T* const poolNew() {
			return static_cast<T*>(pool.alloc());
		}
		inline void poolDelete(T* const deletePtr) {
			deletePtr->~T();
			pool.nextmove(deletePtr);
		}
	private:
		MemoryReuse<T, Size> pool;
	};
}
/*
int
foo() {
	cout << "execute: " << __PRETTY_FUNCTION__ << endl;
	return time(NULL);
}
int mainl()
{
	{//すぐにfooが実行されます。
		cout << "async foo" << endl;
		future<int> f1 = std::async(
			std::launch::async, foo);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		cout << "after sleep" << endl;
		int r = f1.get();
		cout << r << endl;
	}
	cout << endl;

	{//メインスレッドのsleepが終わったあとに、fooが実行されています。
		cout << "async deferred foo" << endl;
		future<int> f1 = std::async(
			std::launch::deferred, foo);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		cout << "after sleep" << endl;
		int r = f1.get();
		cout << r << endl;
	}
	return 0;
}*/

#include <iostream>
#include <functional>
#include <list>
#include <unordered_map>
class EaseAnim
{
private:
	inline float Liner(float start, float end, float value)const
	{
		if (value < 0.0f)value = 0.0f;
		if (value > 1.0f)value = 1.0f;
		return start + end * value;
	}
	inline float spring(float start, float end, float value)const {
		if (value < 0.0f)value = 0.0f;
		if (value > 1.0f)value = 1.0f;
		value = (std::sinf(value * 3.1415926535898f * (0.2f + 2.5f * value * value * value)) * powf(1.0f - value, 2.2f) + value) * (1.0f + (1.2f * (1.0f - value)));
		return start + end * value;
	}
	inline float easeInQuad(float start, float end, float value)const {
		return end * value * value + start;
	}
	inline float easeOutQuad(float start, float end, float value)const {
		return -end * value * (value - 2.0f) + start;
	}
	inline float easeInOutQuad(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1.0f) return end * 0.5f * value * value + start;
		value--;
		return -end * 0.5f * (value * (value - 2.0f) - 1.0f) + start;
	}
	inline float easeInCubic(float start, float end, float value)const {
		return end * value * value * value + start;
	}
	inline float easeOutCubic(float start, float end, float value) const {
		value--;
		return end * (value * value * value + 1.0f) + start;
	}
	inline float easeInOutCubic(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1.0f) return end * 0.5f * value * value * value + start;
		value -= 2.0f;
		return end * 0.5f * (value * value * value + 2.0f) + start;
	}
	inline float easeInQuart(float start, float end, float value) const {
		return end * value * value * value * value + start;
	}
	inline float easeOutQuart(float start, float end, float value)const {
		value--;
		return -end * (value * value * value * value - 1.0f) + start;
	}
	inline float easeInOutQuart(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1.0f) return end * 0.5f * value * value * value * value + start;
		value -= 2.0f;
		return -end * 0.5f * (value * value * value * value - 2.0f) + start;
	}
	inline float easeInQuint(float start, float end, float value)const {
		return end * value * value * value * value * value + start;
	}
	inline float easeOutQuint(float start, float end, float value)const {
		value--;
		return end * (value * value * value * value * value + 1) + start;
	}
	inline float easeInOutQuint(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1.0f) return end * 0.5f * value * value * value * value * value + start;
		value -= 2.0f;
		return end * 0.5f * (value * value * value * value * value + 2.0f) + start;
	}
	inline float easeInSine(float start, float end, float value)const {
		return -end * cosf(value * (3.1415926535898f * 0.5f)) + end + start;
	}
	inline float easeOutSine(float start, float end, float value)const {
		return end * sinf(value * (3.1415926535898f * 0.5f)) + start;
	}
	inline float easeInOutSine(float start, float end, float value)const {
		return -end * 0.5f * (cosf(3.1415926535898f * value) - 1.0f) + start;
	}
	inline float easeInExpo(float start, float end, float value)const {
		return end * powf(2.0f, 10.0f * (value - 1.0f)) + start;
	}
	inline float easeOutExpo(float start, float end, float value)const {
		if (value == 1.0f)return end + start;
		return end * (-powf(2.0f, -10.0f * value) + 1.0f) + start;
	}
	inline float easeInOutExpo(float start, float end, float value)const {
		if (value == 1.0f)return end + start;
		value /= 0.5f;
		if (value < 1.0f) return end * 0.5f * powf(2.0f, 10.0f * (value - 1.0f)) + start;
		value--;
		return end * 0.5f * (-powf(2.0f, -10.0f * value) + 2.0f) + start;
	}
	inline float easeInCirc(float start, float end, float value)const {
		return -end * (sqrt(1.0f - value * value) - 1.0f) + start;
	}
	inline float easeOutCirc(float start, float end, float value)const {
		value--;
		return end * sqrt(1.0f - value * value) + start;
	}
	inline float easeInOutCirc(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1.0f) return -end * 0.5f * (sqrt(1.0f - value * value) - 1.0f) + start;
		value -= 2.0f;
		return end * 0.5f * (sqrt(1 - value * value) + 1.0f) + start;
	}
	inline float easeOutBounce(float start, float end, float value)const {
		if (value < (1.0f / 2.75f)) {
			return end * (7.5625f * value * value) + start;
		}
		else if (value < (2.0f / 2.75f)) {
			value -= (1.5f / 2.75f);
			return end * (7.5625f * (value)*value + 0.75f) + start;
		}
		else if (value < (2.5f / 2.75f)) {
			value -= (2.25f / 2.75f);
			return end * (7.5625f * (value)*value + 0.9375f) + start;
		}
		else {
			value -= (2.625f / 2.75f);
			return end * (7.5625f * (value)*value + 0.984375f) + start;
		}
	}
	inline float easeInBounce(float start, float end, float value)const {
		return end - easeOutBounce(0.0f, end, 1.0f - value) + start;
	}
	inline float easeInOutBounce(float start, float end, float value)const {
		if (value < 1.0f * 0.5f) return easeInBounce(0.0f, end, value * 2.0f) * 0.5f + start;
		else return easeOutBounce(0.0f, end, value * 2.0f - 1.0f) * 0.5f + end * 0.5f + start;
	}
	inline float easeInBack(float start, float end, float value) const {
		return end * (value)*value * ((1.70158f + 1.0f) * value - 1.70158f) + start;
	}
	inline float easeOutBack(float start, float end, float value)const {
		value = value - 1.0f;
		return end * ((value)*value * ((1.70158f + 1.0f) * value + 1.70158f) + 1.0f) + start;
	}
	inline float easeInOutBack(float start, float end, float value)const {
		value /= 0.5f;
		if (value < 1) {
			return end * 0.5f * (value * value * (((1.70158f * 1.525f) + 1.0f) * value - (1.70158f * 1.525f))) + start;
		}
		value -= 2.0f;
		return end * 0.5f * ((value)*value * (((1.70158f * 1.525f) + 1.0f) * value + (1.70158f * 1.525f)) + 2.0f) + start;
	}
	inline float easeInElastic(float start, float end, float value)const {
		float s = 0.0f;
		float a = 0.0f;
		if (value == 0.0f) return start;
		if (value == 1.0f) return start + end;
		if (a == 0.0f || a < abs(end)) {
			a = end;
			s = (1.0f * 0.3f) / 4.0f;
		}
		else {
			s = (1.0f * 0.3f) / (2.0f * 3.1415926535898f) * asinf(end / a);
		}
		return -(a * powf(2.0f, 10.0f * (value -= 1.0f)) * sinf((value - s) * (2.0f * 3.1415926535898f) / (1.0f * 0.3f))) + start;
	}
	inline float easeOutElastic(float start, float end, float value) const {
		float s = 0.0f;
		float a = 0.0f;
		if (value == 0.0f) return start;
		if ((value /= 1.0f) == 1.0f) return start + end;
		if (a == 0.0f || a < abs(end)) {
			a = end;
			s = (1.0f * 0.3f) * 0.25f;
		}
		else {
			s = (1.0f * 0.3f) / (2.0f * 3.1415926535898f) * asin(end / a);
		}
		return (a * powf(2.0f, -10.0f * value) * sinf((value * 1.0f - s) * (2.0f * 3.1415926535898f) / (1.0f * 0.3f)) + end + start);
	}
	inline float easeInOutElastic(float start, float end, float value)const {
		float s = 0.0f;
		float a = 0.0f;
		if (value == 0.0f) return start;
		if ((value /= 1.0f * 0.5f) == 2.0f) return start + end;
		if (a == 0.0f || a < abs(end)) {
			a = end;
			s = (1.0f * 0.3f) / 4.0f;
		}
		else {
			s = (1.0f * 0.3f) / (2.0f * 3.1415926535898f) * asinf(end / a);
		}
		if (value < 1.0f) return -0.5f * (a * powf(2.0f, 10.0f * (value -= 1.0f)) * sinf((value * 1.0f - s) * (2.0f * 3.1415926535898f) / (1.0f * 0.3f))) + start;
		return a * powf(2.0f, -10.0f * (value -= 1.0f)) * sinf((value * 1.0f - s) * (2.0f * 3.1415926535898f) / (1.0f * 0.3f)) * 0.5f + end + start;
	}


};
namespace singleton {
	template<class T> class Singleton {
	public:
		Singleton<T>(const Singleton<T>&) = delete;
		Singleton<T>& operator=(const Singleton<T>&) = delete;
		Singleton<T>(Singleton<T>&&) = delete;
		Singleton<T>& operator=(Singleton<T>&&) = delete;
		static inline T* getInstance() { return instance; }
	protected:
		// コンストラクタ  
		explicit inline Singleton(T* set) { instance = set; }
		// デストラクタ  
		virtual inline ~Singleton() throw() { instance = nullptr; }
	private:
		static T* instance;
	}; template<class T> T* Singleton<T>::instance = nullptr;
}
class Command : public singleton::Singleton<Command>
{
public:
	static void Init() { static Command d;  }
	void Update();
	void Draw();
	const bool IsKeyState(int vKey);
	const bool IsKeyStatePrev(int vKey);
	const bool IsHoldingKey(int vKey);
	const bool IsReleasingKey(int vKey);
	const bool WasKeyPressed(int vKey);
	const bool HasReleasedKey(int vKey);
	const int InputtimeCountKey(int vKey);
	const bool Anyinput() { return anyinput; }
	 int getMouseX() const { return m_mouseX; }
	 int getMouseY() const { return m_mouseY; }
	 double getTime() const { return m_fTime; }
	void TimerReset() { timerReset = true; }
	 float getElapsedTime() const { return m_fElapsedTime; }
	 float getElapsedTime50() const { return m_fElapsedTime * 50.0f; }
	int GetPressingCount(int keyCode);//keyCodeのキーが押されているフレーム数を取得
	void UpdateMouseInfo();
	void UpdatekeyboardInfo();
	void UpdateTimer();
	const static int LEFT_M = 0;
	const static int RIGHT_M = 1;
	const static int MIDDLE_M = 2;
	enum ClickState {
		IsClicked,
		IsHoldClicked,
		IsRelesClicked,
		IsRelesClicking,
	};
	inline ClickState getClickState() const {
		return state;
	}
private:
	Command();
	~Command();
	enum Key
	{
		A = 1,
		B = 2,
		X = 4,
		Y = 8,
		S = 16,
		C = 32,
		UP = 64,
		DOWN = 128,
		LEFT = 256,
		RIGHT = 512,
		SPACE = 1024,
		UpLeft = UP | LEFT,
		UpRight = UP | RIGHT,
		DownLeft = DOWN | LEFT,
		DownRight = DOWN | RIGHT,
		RIGHT_X = RIGHT | X
	};

	struct Move
	{
		TCHAR Name[64];
		// この動作をアクティブ化するために使用する入力がより長い動作の
		// コンポーネントとして再利用できる場合、これを true に設定します。
		//(trueが通常技 falseが特殊技)
		bool IsSubMove;
		// この動作をアクティブ化するために必要なボタン押下のシーケンス。
		std::vector< int> Sequence;
		size_t Size;
		Move(const TCHAR* Name, std::vector< int> Keys, bool IsSubMove) : Sequence(Keys), IsSubMove(IsSubMove)
		{
			lstrcpy(this->Name, Name);
			Size = Keys.size();
		}
	};

	ClickState state;
	const static int BUTTON_NUM = 8;
	int mKeyPressingCount[BUTTON_NUM];//押されカウンタ
	int m_mouseX, m_mouseY;

	double m_fTime;
	float m_fElapsedTime;

	LARGE_INTEGER qwTime;
	LONGLONG m_llBaseTime;
	double m_llQPFTicksPerSec;
	LONGLONG m_llLastElapsedTime;

	bool timerReset;
private:
	float timercount;
	bool anyinput;
	int newkey[256];
	bool Prevkey[256];
	const TCHAR* AttackName;
	std::vector< Move> moves;
	std::list< int> Buffer;
private:
	void CommandUpdate();
	void Add();
	bool Matches(Move move);
	const TCHAR* DetectMove();
};
Command::Command()
	: Singleton(this),
	  anyinput(false)
	, timercount(0.0f)
	, AttackName("")
	, m_mouseX(0)
	, m_mouseY(0)
	, m_fTime(0.0)
	, m_fElapsedTime(0.0166f)
	, state(IsRelesClicked)
{
	memset(Prevkey, false, sizeof(Prevkey));
	memset(newkey, 0, sizeof(newkey));
	memset(mKeyPressingCount, 0, sizeof(mKeyPressingCount));

	LARGE_INTEGER qwTicksPerSec = { 0 };
	QueryPerformanceFrequency(&qwTicksPerSec);
	m_llQPFTicksPerSec = static_cast<double>(qwTicksPerSec.QuadPart);
	// Get the current time
	qwTime = { 0 };
	QueryPerformanceCounter(&qwTime);
	m_llLastElapsedTime = qwTime.QuadPart;
	m_llBaseTime = qwTime.QuadPart;
	timerReset = false;
	Add();
}
Command::~Command()
{
}
void Command::Add()
{
	Buffer.clear();
	moves.clear();

	moves = {
		{ "Jump",{ A }, true },
		{ "Punch",{ X }, true },
		{ "Guard",{ DOWN }, true },
		{ "Double Punch",{ X,X }, true },
		{ "Punch Punch",{ X,X,X }, true },

		{ "SP Punch",{ X,X,X,X }, false },
		{ "Double Jump",{ A,A }, false },
		{ "Jump Kick",{ A | X }, false },
		{ "Quad Punch",{ X,Y,X,Y }, false },
		{ "Firebal",{ DOWN,DownRight,RIGHT_X }, false },
		{ "Long Jump",{ UP,UP,A }, false },
		{ "Dash",{ RIGHT,RIGHT }, false },
		{ "Dash",{ LEFT,LEFT }, false },
		{ "Tornado",{ RIGHT,LEFT,RIGHT,LEFT,RIGHT,LEFT }, false },
		{ "Tornado",{ LEFT,RIGHT,LEFT,RIGHT,LEFT,RIGHT }, false },
		{ "Back Flip",{ DOWN,DOWN,A }, false },
		{ "30 Lives",{ UP,UP,DOWN,DOWN,LEFT,RIGHT,LEFT,RIGHT,B,A }, false }
	};
}
bool Command::Matches(Move move)
{
	// 動作がバッファーよりも長い場合、多分一致できません。
	if (Buffer.size() < move.Size)
		return false;

	// 最新の入力に一致するように逆方向にループします。
	std::vector< int>::const_reverse_iterator itr = move.Sequence.rbegin();
	std::list< int>::const_reverse_iterator itr2 = Buffer.rbegin();
	while (itr != move.Sequence.rend())
	{
		if ((*itr) != (*itr2)) return false;

		itr++;
		itr2++;
	}
	// この動作が大きなシーケンスのコンポーネントでない限り、
	if (!move.IsSubMove)
	{
		// 使用済みの入力を使用します。
		Buffer.clear();
	}
	return true;
}
const TCHAR* Command::DetectMove()
{
	std::vector< Move>::const_reverse_iterator itr = moves.rbegin();
	while (itr != moves.rend())
	{
		if (Matches((*itr))) {
			return (*itr).Name;
		}
		itr++;
	}
	return "";
}
void Command::CommandUpdate()
{
	// まったく同じフレームで 2 つのボタンを押すことは非常に困難です。
	// ほとんど差がない場合は、同時に押されたものとみなします。
	bool mergeInput = (Buffer.size() > 0 && timercount < 0.1f);

	int buttons = 0;
	if (newkey[KEY_INPUT_A] == 1)buttons |= A;
	if (newkey[KEY_INPUT_B] == 1)buttons |= B;
	if (newkey[KEY_INPUT_X] == 1)buttons |= X;
	if (newkey[KEY_INPUT_Y] == 1)buttons |= Y;
	if (newkey[KEY_INPUT_S] == 1)buttons |= S;
	if (newkey[KEY_INPUT_C] == 1)buttons |= C;
	if (newkey[KEY_INPUT_UP] == 1)buttons |= UP;
	if (newkey[KEY_INPUT_DOWN] == 1)buttons |= DOWN;
	if (newkey[KEY_INPUT_RIGHT] == 1)buttons |= RIGHT;
	if (newkey[KEY_INPUT_LEFT] == 1)buttons |= LEFT;
	if (newkey[KEY_INPUT_SPACE] == 1)buttons |= SPACE;
	if (buttons) {
		if (mergeInput && (Buffer.back() != buttons))
		{
			Buffer.back() |= buttons;
		}
		else {
			Buffer.push_back(buttons);
			timercount = 0.0f;//タイマーリセット
		}

		AttackName = DetectMove();
	}
}
void Command::UpdateMouseInfo()
{
	int nowInput = GetMouseInput();
	for (int i = 0; i < BUTTON_NUM; i++) {
		if ((nowInput >> i) & 0x01) {
			mKeyPressingCount[i]++;
		}
		else {
			if (mKeyPressingCount[i] > 0) {
				mKeyPressingCount[i] = 0;
			}
		}
	}
	(mKeyPressingCount[Command::LEFT] == 0) ?
		((state == IsHoldClicked) ? state = IsRelesClicking : state = IsRelesClicked) :
		((state == IsRelesClicked) ? state = IsClicked : state = IsHoldClicked);
}
void Command::UpdatekeyboardInfo()
{
	char buf[256];
	GetHitKeyStateAll(buf);

	for (int i = 0; i < 256; i++) {
		Prevkey[i] = newkey[i] > 0;
		buf[i] ? newkey[i]++ : newkey[i] = 0;
	}
}
void Command::UpdateTimer()
{
	QueryPerformanceCounter(&qwTime);
	if (timerReset) {
		m_llBaseTime = qwTime.QuadPart;
		timerReset = false;
	}
	m_fTime = (qwTime.QuadPart - m_llBaseTime) / m_llQPFTicksPerSec;
	m_fElapsedTime = static_cast<float>((qwTime.QuadPart - m_llLastElapsedTime) / m_llQPFTicksPerSec);
	m_llLastElapsedTime = qwTime.QuadPart;
}
void Command::Update()
{
	UpdateTimer();

	timercount += m_fElapsedTime;

	if (timercount > 0.5f && Buffer.size() > 0) {
		Buffer.clear();
	}
	GetMousePoint(&m_mouseX, &m_mouseY);
	if (CheckHitKeyAll(DX_CHECKINPUT_KEY) == 0x00) {
		if (!anyinput) return;
		anyinput = false;
	}
	UpdateMouseInfo();
	UpdatekeyboardInfo();
	CommandUpdate();

	if (!anyinput)
		anyinput = true;
}
void Command::Draw()
{
	TCHAR str[32];
	int count = 0;
	std::list< int>::const_iterator itr = Buffer.begin();
	while (itr != Buffer.end())
	{
		switch ((*itr))
		{
		case A: lstrcpy(str, "A"); break;
		case B:lstrcpy(str, "B"); break;
		case X:lstrcpy(str, "X"); break;
		case Y:lstrcpy(str, "Y"); break;
		case S:lstrcpy(str, "S"); break;
		case C:lstrcpy(str, "C"); break;
		case UP:lstrcpy(str, "UP"); break;
		case DOWN:lstrcpy(str, "DOWN"); break;
		case RIGHT:lstrcpy(str, "RIGHT"); break;
		case LEFT:lstrcpy(str, "LEFT"); break;
		case SPACE:lstrcpy(str, "SPACE"); break;
		case UpLeft:lstrcpy(str, "UpLeft"); break;
		case UpRight:lstrcpy(str, "UpRight"); break;
		case DownLeft:lstrcpy(str, "DownLeft"); break;
		case DownRight:lstrcpy(str, "DownRight"); break;
		case RIGHT_X:lstrcpy(str, "RIGHT_X"); break;
		default:lstrcpy(str, "*"); break;
		}
		DrawFormatString(0, 60 + (count * 15), GetColor(255, 255, 255), str);
		count++;

		itr++;
	}
	DrawFormatString(128, 60, GetColor(255, 255, 255), AttackName);

	
}
// キーボード情報
const bool Command::IsKeyState(int vKey)
{
	return newkey[vKey] > 0;
}
const bool Command::IsKeyStatePrev(int vKey)
{
	return Prevkey[vKey];
}
const bool Command::IsHoldingKey(int vKey)
{
	return Prevkey[vKey] && newkey[vKey] > 0;
}
const bool Command::IsReleasingKey(int vKey)
{
	return !Prevkey[vKey] && newkey[vKey] == 0;
}
const bool Command::WasKeyPressed(int vKey)
{
	return !Prevkey[vKey] && newkey[vKey] > 0;
}
const bool Command::HasReleasedKey(int vKey)
{
	return Prevkey[vKey] && newkey[vKey] == 0;
}
const int Command::InputtimeCountKey(int vKey)
{
	return newkey[vKey];
}
//keyCodeのキーが押されているフレーム数を返す
int Command::GetPressingCount(int keyCode) {
	return mKeyPressingCount[keyCode];
}


namespace dxrx {

	interface IDisposable {
		bool disable();
	};
	template <class Type>
	interface IObserver {
		void OnCompleted();
		void OnError();
		void OnNext(Type value, BOOL& error);
	};
	template <class Type>
	interface IObservable {
		IDisposable Subscribe(IObserver<Type>* observer);
	};
	template <class TSource, class TResult>
	interface ISubject : IObserver<TSource>, IObservable<TResult>
	{

	};
	template <class T>
	interface ISubject2 : ISubject<T, T>, IObserver<T>, IObservable<T>
	{

	};
	template <class TKey, class TElement>
	interface IGroupedObservable : IObservable<TElement>
	{
		TKey Key() { return key; }
	private:
		TKey key;
	};


	template <class T>
	class MyFilter :public IObservable<T>
	{
	public:
		CRITICAL_SECTION    g_cs;
		class Lock
		{
		public:
#pragma prefast( suppress:26166, "g_bThreadSafe controls behavior" )
			inline _Acquires_lock_(g_cs) Lock() { EnterCriticalSection(&g_cs); }
#pragma prefast( suppress:26165, "g_bThreadSafe controls behavior" )
			inline _Releases_lock_(g_cs) ~Lock() { LeaveCriticalSection(&g_cs); }
		};
		/// <summary>
		/// 上流となるObservable
		/// </summary>
		IObservable<T> _source;

		/// <summary>
		/// 判定式
		/// </summary>
		std::function<bool(T)> _conditionalFunc;

		MyFilter(IObservable<T> source, std::function<bool(T)> conditionalFunc)
		{
			_source = source;
			_conditionalFunc = conditionalFunc;
		}

		IDisposable Subscribe(IObserver<T>*observer)
		{
			//Subscribeされたら、MyFilterOperator本体を作って返却
			return MyFilterInternal(this, observer).Run();
		}

		// ObserverとしてMyFilterInternalが実際に機能する
		class MyFilterInternal : IObserver<T>
		{
			MyFilter<T>* _parent;
			IObserver<T>* _observer;

			MyFilterInternal(MyFilter<T>* parent, IObserver<T>* observer)
			{
				_observer = observer;
				_parent = parent;
			}

			IDisposable Run()
			{
				return _parent->_source.Subscribe(this);
			}

			void OnNext(T value, BOOL& error)
			{
				Lock l;

				if (_observer == nullptr) return;
				
				//条件を満たす場合のみOnNextを通過
				if (_parent->_conditionalFunc(value))
				{
					_observer->OnNext(value, error);
					if (error)
					{
						//途中でエラーが発生したらエラーを送信
						_observer->OnError();
						_observer = nullptr;
					}
				}
			}

			void OnError()
			{
				Lock l;

				//エラーを伝播して停止
				_observer->OnError();
				_observer = nullptr;

			}

			void OnCompleted()
			{
				Lock l;

				//停止
				_observer->OnCompleted();
				_observer = nullptr;

			}
		};
	};
	template <class T>
	class ObservableOperators
	{
		IObservable<T> FilterOperator(IObservable<T>* source, std::function<bool(T)> conditionalFunc)
		{
			return MyFilter<T>(source, conditionalFunc);
		}
	};
}

#include <chrono>
namespace s3d
{	/// <summary>
	/// 日数 (int32)
	/// Days (int32)
	/// </summary>
	using Days = std::chrono::duration<INT32, std::ratio<86400>>;

	/// <summary>
	/// 日数 (double)
	/// Days (double)
	/// </summary>
	using DaysF = std::chrono::duration<double, std::ratio<86400>>;


	/// <summary>
	/// 時間 (int32)
	/// Hours (int32)
	/// </summary>
	using Hours = std::chrono::hours;

	/// <summary>
	/// 時間 (double)
	/// Hours (double)
	/// </summary>
	using HoursF = std::chrono::duration<double, std::ratio<3600>>;


	/// <summary>
	/// 分 (int32)
	/// Minutes (int32)
	/// </summary>
	using Minutes = std::chrono::minutes;

	/// <summary>
	/// 分 (double)
	/// Minutes (double)
	/// </summary>
	using MinutesF = std::chrono::duration<double, std::ratio<60>>;


	/// <summary>
	/// 秒 (int64)
	/// Seconds (int64)
	/// </summary>
	using Seconds = std::chrono::seconds;

	/// <summary>
	/// 秒 (double)
	/// Seconds (double)
	/// </summary>
	using SecondsF = std::chrono::duration<double>;


	/// <summary>
	/// ミリ秒 (int64)
	/// Milliseconds (int64)
	/// </summary>
	using Milliseconds = std::chrono::milliseconds;

	/// <summary>
	/// ミリ秒 (double)
	/// Milliseconds (double)
	/// </summary>
	using MillisecondsF = std::chrono::duration<double, std::milli>;


	/// <summary>
	/// マイクロ秒 (int64)
	/// Microseconds (int64)
	/// </summary>
	using Microseconds = std::chrono::microseconds;

	/// <summary>
	/// マイクロ秒 (double)
	/// Microseconds (double)
	/// </summary>
	using MicrosecondsF = std::chrono::duration<double, std::micro>;


	/// <summary>
	/// ナノ秒 (int64)
	/// Nanoseconds (int64)
	/// </summary>
	using Nanoseconds = std::chrono::nanoseconds;

	/// <summary>
	/// ナノ秒 (double)
	/// Nanoseconds (double)
	/// </summary>
	using NanosecondsF = std::chrono::duration<double, std::nano>;
	using uint64 = UINT64;
	
	using Duration = SecondsF;
	namespace Rx
	{
		template <class Type>
		struct Message
		{
			uint64 timeMicrosec;

			Type value;
		};

		template <class Type, bool continuous>
		class IOperator
		{
		protected:

			virtual IOperator& setChild(const std::shared_ptr<IOperator<Type, continuous>>& child) = 0;

		public:

			virtual ~IOperator() = default;

			virtual void send(const Message<Type>& message) = 0;

			virtual void update(const uint64) = 0;

			IOperator<Type, continuous>& delay(const int delayMicrosec);

			IOperator<Type, continuous>& distinctUntilChanged();

			IOperator<Type, continuous>& filter(std::function<bool(Type)> filterFunction);

			void subscribe(std::function<void(Type)> func);
		};

		template <class Type, bool continuous>
		class DelayStream : public IOperator<Type, continuous>
		{
		private:

			std::vector<Message<Type>> m_stream;

			Message<Type> m_lastSentMessage;

			UINT64 m_delayTimeMicrosec = 0;

			std::shared_ptr<IOperator<Type, continuous>> m_child;

			IOperator<Type, continuous>& setChild(const std::shared_ptr<IOperator<Type, continuous>>& child) override
			{
				return *(m_child = child);
			}

		public:

			explicit DelayStream(UINT64 delayTimeMicrosec)
				: m_delayTimeMicrosec(delayTimeMicrosec) {}

			void send(const Message<Type>& message) override
			{
				m_stream.push_back(message);
			}

			void update(const uint64 currentTimeMicrosec) override
			{
				if constexpr (continuous)
				{
					for (auto it = m_stream.begin(); it != m_stream.end();)
					{
						if ((currentTimeMicrosec - it->timeMicrosec) >= m_delayTimeMicrosec)
						{
							m_lastSentMessage = *it;

							it = m_stream.erase(it);
						}
						else
						{
							++it;
						}
					}

					if (m_child && m_lastSentMessage)
					{
						m_child->send(*m_lastSentMessage);
					}
				}
				else
				{
					for (auto it = m_stream.begin(); it != m_stream.end();)
					{
						if ((currentTimeMicrosec - it->timeMicrosec) >= m_delayTimeMicrosec)
						{
							if (m_child)
							{
								m_child->send(*it);
							}

							m_lastSentMessage = *it;

							it = m_stream.erase(it);
						}
						else
						{
							++it;
						}
					}
				}

				if (m_child)
				{
					m_child->update(currentTimeMicrosec);
				}
			}
		};

		template <class Type, bool continuous>
		class DistinctUntilChanged : public IOperator<Type, continuous>
		{
		private:

			std::vector<Message<Type>> m_stream;

			Message<Type> m_lastSentMessage;

			std::shared_ptr<IOperator<Type, continuous>> m_child;

			IOperator<Type, continuous>& setChild(const std::shared_ptr<IOperator<Type, continuous>>& child) override
			{
				return *(m_child = child);
			}

		public:

			DistinctUntilChanged() = default;

			void send(const Message<Type>& message) override
			{
				m_stream.push_back(message);
			}

			void update(const uint64 currentTimeMicrosec) override
			{
				for (auto it = m_stream.begin(); it != m_stream.end();)
				{
					if (m_lastSentMessage && it->value == m_lastSentMessage->value)
					{
						it = m_stream.erase(it);
					}
					else
					{
						m_child->send(*it);

						m_lastSentMessage = *it;

						it = m_stream.erase(it);

						m_child->update(currentTimeMicrosec);
					}
				}
			}
		};

		template <class Type, bool continuous>
		class Filter : public IOperator<Type, continuous>
		{
		private:

			std::shared_ptr<IOperator<Type, continuous>> m_child;

			std::function<bool(Type)> m_filterFunction;

			bool m_updated = false;

			IOperator<Type, continuous>& setChild(const std::shared_ptr<IOperator<Type, continuous>>& child) override
			{
				return *(m_child = child);
			}

		public:

			Filter() = default;

			explicit Filter(std::function<bool(Type)> filterFunction)
				: m_filterFunction(filterFunction) {}

			void send(const Message<Type>& message) override
			{
				if (!m_child)
				{
					return;
				}

				if (m_filterFunction && m_filterFunction(message.value))
				{
					m_child->send(message);

					m_updated = true;
				}
			}

			void update(const uint64 currentTimeMicrosec) override
			{
				if (!m_child)
				{
					return;
				}

				if (m_child && m_updated)
				{
					m_child->update(currentTimeMicrosec);

					m_updated = false;
				}
			}
		};

		template <class Type, bool continuous>
		class Subscriber : public IOperator<Type, continuous>
		{
		private:

			std::vector<Message<Type>> m_stream;

			using function_type = std::function<void(Type)>;

			function_type m_function;

			IOperator<Type, continuous>& setChild(const std::shared_ptr<IOperator<Type, continuous>>&) override
			{
				return *this;
			}

		public:

			Subscriber() = default;

			explicit Subscriber(function_type function)
				: m_function(function) {}

			void send(const Message<Type>& message) override
			{
				m_stream.push_back(message);
			}

			void update(const uint64) override
			{
				if (m_function)
				{
					for (const auto& stream : m_stream)
					{
						m_function(stream.value);
					}
				}

				m_stream.clear();
			}
		};

		template <class Type, bool continuous>
		IOperator<Type, continuous>& IOperator<Type, continuous>::delay(const int delayMicrosec)
		{
			return setChild(std::make_shared<DelayStream<Type, continuous>>(static_cast<UINT64>(std::chrono::milliseconds(delayMicrosec).count())));
		}

		template <class Type, bool continuous>
		IOperator<Type, continuous>& IOperator<Type, continuous>::distinctUntilChanged()
		{
			return setChild(std::make_shared<DistinctUntilChanged<Type, continuous>>());
		}

		template <class Type, bool continuous>
		IOperator<Type, continuous>& IOperator<Type, continuous>::filter(std::function<bool(Type)> filterFunction)
		{
			return setChild(std::make_shared<Filter<Type, continuous>>(filterFunction));
		}

		template <class Type, bool continuous>
		void IOperator<Type, continuous>::subscribe(std::function<void(Type)> func)
		{
			setChild(std::make_shared<Subscriber<Type, continuous>>(func));
		}

		template <class Type, Type(*func)(), bool continuous>
		class InputStream
		{
		private:

			std::vector<std::shared_ptr<IOperator<Type, continuous>>> m_children;

		public:

			IOperator<Type, continuous>& delay(const MicrosecondsF& delayMicrosec)
			{
				m_children.push_back(std::make_shared<DelayStream<Type, continuous>>(static_cast<UINT64>(delayMicrosec.count())));

				return *m_children.back();
			}

			IOperator<Type, continuous>& distinctUntilChanged()
			{
				m_children.push_back(std::make_shared<DistinctUntilChanged<Type, continuous>>());

				return *m_children.back();
			}

			IOperator<Type, continuous>& filter(std::function<bool(Type)> filterFunction)
			{
				m_children.push_back(std::make_shared<Filter<Type, continuous>>(filterFunction));

				return *m_children.back();
			}
			[[nodiscard]] uint64 GetMicrosec();
			void subscribe(std::function<void(Type)> function)
			{
				m_children.push_back(std::make_shared<Subscriber<Type, continuous>>(function));
			}

			void send(const Message<Type>& message)
			{
				for (auto& child : m_children)
				{
					child->send(message);
				}
			}

			void update()
			{
				const uint64 currentTimeMicrosec /*= Time::GetMicrosec()*/;

				for (auto& child : m_children)
				{
					child->send(Message<Type>{ currentTimeMicrosec, func() });

					child->update(currentTimeMicrosec);
				}
			}
		};
	}
}

/*
イベントID(long +=1)取得
すぐに発行
一定時間後に値を発行したい
一定間隔で値を発行したい
一回~処理
永続的に処理
一定フレーム間隔で値を発行したい
処理開始時
処理終了時
条件式を満たすものだけ通したい
値が変化した時のみ通したい
メッセージを時間遅延させたい
次のフレームで処理がしたい
非同期処理
エラー処理
テスト用
強制終了
破棄
カスタマイズ用
*/
namespace IKD
{
	using namespace std;
	template <class T>
	class CCell;
	/////////////////////////////////////
	// 分木登録オブジェクト(OFT)
	//////////////////////////////////
	template< class T>
	class OBJECT_FOR_TREE
	{
	public:
		CCell<T>* m_pCell;			// 登録空間
		T* m_pObject;				// 判定対象オブジェクト
		OBJECT_FOR_TREE<T>* m_spPre;	// 前のOBJECT_FOR_TREE構造体
		OBJECT_FOR_TREE<T>* m_spNext;	// 次のOBJECT_FOR_TREE構造体

	public:
		OBJECT_FOR_TREE() {
			m_pCell = NULL;
			m_pObject = NULL;
			m_spPre = nullptr;
			m_spNext = nullptr;
		}

		virtual ~OBJECT_FOR_TREE()
		{

		}

	public:
		// 自らリストから外れる
		bool Remove() {
			// すでに逸脱している時は処理終了
			if (!m_pCell)
				return false;

			// 自分を登録している空間に自身を通知
			if (!m_pCell->OnRemove(this))
				return false;

			// 逸脱処理
			// 前後のオブジェクトを結びつける
			if (m_spPre != NULL)
			{
				m_spPre->m_spNext = m_spNext;
				m_spPre = (NULL);
			}
			if (m_spNext != NULL)
			{
				m_spNext->m_spPre = m_spPre;
				m_spNext = (NULL);
			}
			m_pCell = NULL;
			return true;
		}

		// 空間を登録
		void RegistCell(CCell<T>* pCell)
		{
			m_pCell = pCell;
		}

		// 次のオブジェクトへのスマートポインタを取得
		OBJECT_FOR_TREE<T>*& GetNextObj() {
			return m_spNext;
		}
	};



	/////////////////////////////////////
	// 線形4分木空間管理クラス
	//////////////////////////////////
#define CLINER4TREEMANAGER_MAXLEVEL		9
	template <class T>
	class CLiner4TreeManager
	{
	protected:
		unsigned int m_uiDim;
		CCell<T>** ppCellAry;	// 線形空間ポインタ配列
		unsigned int m_iPow[CLINER4TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列
		float m_fW;		// 領域のX軸幅
		float m_fH;		// 領域のY軸幅
		float m_fLeft;	// 領域の左側（X軸最小値）
		float m_fTop;	// 領域の上側（Y軸最小値）
		float m_fUnit_W;		// 最小レベル空間の幅単位
		float m_fUnit_H;		// 最小レベル空間の高単位
		DWORD m_dwCellNum;		// 空間の数
		unsigned int m_uiLevel;			// 最下位レベル

	public:
		// コンストラクタ
		CLiner4TreeManager()
		{
			m_uiLevel = 0;
			m_fW = 0.0f;
			m_fH = 0.0f;
			m_fLeft = 0.0f;
			m_fTop = 0.0f;
			m_fUnit_W = 0.0f;
			m_fUnit_H = 0.0f;
			m_dwCellNum = 0;
			ppCellAry = NULL;
			m_uiDim = 0;
		}

		// デストラクタ
		virtual ~CLiner4TreeManager()
		{
			CellDelete();
		}
		void CellDelete() {
			if (ppCellAry) {
				for (DWORD i = 0; i < m_dwCellNum; i++) {
					if (ppCellAry[i] != nullptr)
						delete ppCellAry[i];
				}
				delete[] ppCellAry;
			}
		}
		// 線形4分木配列を構築する
		bool Init(unsigned int Level, float left, float top, float right, float bottom)
		{
			// 設定最高レベル以上の空間は作れない
			if (Level >= CLINER4TREEMANAGER_MAXLEVEL)
				return false;

			// 各レベルでの空間数を算出
			int i;
			m_iPow[0] = 1;
			for (i = 1; i < CLINER4TREEMANAGER_MAXLEVEL + 1; i++)
				m_iPow[i] = m_iPow[i - 1] * 4;

			// Levelレベル（0基点）の配列作成
			m_dwCellNum = (m_iPow[Level + 1] - 1) / 3;
			ppCellAry = NEW CCell<T>*[m_dwCellNum];
			ZeroMemory(ppCellAry, sizeof(CCell<T>*) * m_dwCellNum);

			// 領域を登録
			m_fLeft = left;
			m_fTop = top;
			m_fW = right - left;
			m_fH = bottom - top;
			m_fUnit_W = m_fW / (1 << Level);
			m_fUnit_H = m_fH / (1 << Level);

			m_uiLevel = Level;

			return true;
		}

		// オブジェクトを登録する
		bool Regist(float left, float top, float right, float bottom, OBJECT_FOR_TREE<T>*& spOFT)
		{
			// オブジェクトの境界範囲から登録モートン番号を算出
			DWORD Elem = GetMortonNumber(left, top, right, bottom);
			if (Elem < m_dwCellNum) {
				// 空間が無い場合は新規作成
				if (!ppCellAry[Elem])
					CreateNewCell(Elem);
				return ppCellAry[Elem]->Push(spOFT);
			}
			return false;	// 登録失敗
		}

		// 衝突判定リストを作成する
		DWORD GetAllCollisionList(vector<T*>& ColVect)
		{
			// リスト（配列）は必ず初期化します
			ColVect.clear();

			// ルート空間の存在をチェック
			if (ppCellAry[0] == NULL)
				return 0;	// 空間が存在していない

							// ルート空間を処理
			list<T*> ColStac;
			GetCollisionList(0, ColVect, ColStac);

			return (DWORD)ColVect.size();
		}



	protected:
		// 空間内で衝突リストを作成する
		bool GetCollisionList(DWORD Elem, vector<T*>& ColVect, list<T*>& ColStac)
		{
			typename list<T*>::iterator it;
			// ① 空間内のオブジェクト同士の衝突リスト作成
			OBJECT_FOR_TREE<T>* spOFT1 = ppCellAry[Elem]->GetFirstObj();
			while (spOFT1 != NULL)
			{
				OBJECT_FOR_TREE<T>* spOFT2 = spOFT1->m_spNext;
				while (spOFT2 != NULL) {
					// 衝突リスト作成
					ColVect.push_back(spOFT1->m_pObject);
					ColVect.push_back(spOFT2->m_pObject);
					spOFT2 = spOFT2->m_spNext;
				}
				// ② 衝突スタックとの衝突リスト作成
				for (it = ColStac.begin(); it != ColStac.end(); it++) {
					ColVect.push_back(spOFT1->m_pObject);
					ColVect.push_back(*it);
				}
				spOFT1 = spOFT1->m_spNext;
			}

			bool ChildFlag = false;
			// ③ 子空間に移動
			DWORD ObjNum = 0;
			DWORD i, NextElem;
			for (i = 0; i < 4; i++) {
				NextElem = Elem * 4 + 1 + i;
				if (NextElem < m_dwCellNum && ppCellAry[Elem * 4 + 1 + i]) {
					if (!ChildFlag) {
						// ④ 登録オブジェクトをスタックに追加
						spOFT1 = ppCellAry[Elem]->GetFirstObj();
						while (spOFT1) {
							ColStac.push_back(spOFT1->m_pObject);
							ObjNum++;
							spOFT1 = spOFT1->m_spNext;
						}
					}
					ChildFlag = true;
					GetCollisionList(Elem * 4 + 1 + i, ColVect, ColStac);	// 子空間へ
				}
			}

			// ⑤ スタックからオブジェクトを外す
			if (ChildFlag) {
				for (i = 0; i < ObjNum; i++)
					ColStac.pop_back();
			}

			return true;
		}


		// 空間を生成
		bool CreateNewCell(DWORD Elem)
		{
			// 引数の要素番号
			while (!ppCellAry[Elem])
			{
				// 指定の要素番号に空間を新規作成
				ppCellAry[Elem] = NEW CCell<T>;

				// 親空間にジャンプ
				Elem = (Elem - 1) >> 2;
				if (Elem >= m_dwCellNum) break;
			}
			return true;
		}

		// 座標から空間番号を算出
		DWORD GetMortonNumber(float left, float top, float right, float bottom)
		{
			// 最小レベルにおける各軸位置を算出
			DWORD LT = GetPointElem(left, top);
			DWORD RB = GetPointElem(right, bottom);

			// 空間番号を引き算して
			// 最上位区切りから所属レベルを算出
			DWORD Def = RB - LT;
			unsigned int HiLevel = 0;
			unsigned int i;
			for (i = 0; i < m_uiLevel; i++)
			{
				DWORD Check = (Def >> (i * 2)) & 0x3;
				if (Check != 0)
					HiLevel = i + 1;
			}
			DWORD SpaceNum = RB >> (HiLevel * 2);
			DWORD AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
			SpaceNum += AddNum;

			if (SpaceNum > m_dwCellNum)
				return 0xffffffff;

			return SpaceNum;
		}

		// ビット分割関数
		DWORD BitSeparate32(DWORD n)
		{
			n = (n | (n << 8)) & 0x00ff00ff;
			n = (n | (n << 4)) & 0x0f0f0f0f;
			n = (n | (n << 2)) & 0x33333333;
			return (n | (n << 1)) & 0x55555555;
		}

		// 2Dモートン空間番号算出関数
		WORD Get2DMortonNumber(WORD x, WORD y)
		{
			return (WORD)(BitSeparate32(x) | (BitSeparate32(y) << 1));
		}

		// 座標→線形4分木要素番号変換関数
		DWORD GetPointElem(float pos_x, float pos_y)
		{
			return Get2DMortonNumber((WORD)((pos_x - m_fLeft) / m_fUnit_W), (WORD)((pos_y - m_fTop) / m_fUnit_H));
		}
	};
	/////////////////////////////////////
	// 線形8分木空間管理クラス
	//////////////////////////////////
#define CLINER8TREEMANAGER_MAXLEVEL		7
	template <class T>
	class CLiner8TreeManager
	{
	protected:
		unsigned int m_uiDim;
		CCell<T>** ppCellAry;	// 線形空間ポインタ配列
		unsigned int m_iPow[CLINER8TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列
		VECTOR m_W;	// 領域の幅
		VECTOR m_RgnMin;	// 領域の最小値
		VECTOR m_RgnMax;	// 領域の最大値
		VECTOR m_Unit;		// 最小領域の辺の長さ
		DWORD m_dwCellNum;		// 空間の数
		unsigned int m_uiLevel;			// 最下位レベル

	public:
		// コンストラクタ
		CLiner8TreeManager()
		{
			m_uiDim = 0;
			ppCellAry = NULL;
			m_W.x = m_W.y = m_W.z = 1.0f;
			m_RgnMin.x = m_RgnMin.y = m_RgnMin.z = 0.0f;
			m_RgnMax.x = m_RgnMax.y = m_RgnMax.z = 1.0f;
			m_Unit.x = m_Unit.y = m_Unit.z = 1.0f;
			m_dwCellNum = 0;
			m_uiLevel = 0;
		}

		// デストラクタ
		virtual ~CLiner8TreeManager()
		{

		}
		void CellDelete() {
			if (ppCellAry) {
				for (DWORD i = 0; i < m_dwCellNum; i++) {
					if (ppCellAry[i] != nullptr)
						delete ppCellAry[i];
				}
				delete[] ppCellAry;
				ppCellAry = nullptr;
			}
		}
		// 線形8分木配列を構築する
		bool Init(unsigned int Level, VECTOR& Min, VECTOR& Max)
		{
			// 設定最高レベル以上の空間は作れない
			if (Level >= CLINER8TREEMANAGER_MAXLEVEL)
				return false;

			// 各レベルでの空間数を算出
			int i;
			m_iPow[0] = 1;
			for (i = 1; i < CLINER8TREEMANAGER_MAXLEVEL + 1; i++)
				m_iPow[i] = m_iPow[i - 1] * 8;

			// Levelレベル（0基点）の配列作成
			m_dwCellNum = (m_iPow[Level + 1] - 1) / 7;
			ppCellAry = NEW CCell<T>*[m_dwCellNum];
			ZeroMemory(ppCellAry, sizeof(CCell<T>*) * m_dwCellNum);

			// 領域を登録
			m_RgnMin = Min;
			m_RgnMax = Max;
			m_W = VSub(m_RgnMax, m_RgnMin);
			const float l = static_cast<float>(1 << Level);
			m_Unit = VGet(m_W.x / l, m_W.y / l, m_W.z / l);

			m_uiLevel = Level;

			return true;
		}

		// オブジェクトを登録する
		bool Regist(VECTOR* Min, VECTOR* Max, OBJECT_FOR_TREE<T>*& spOFT)
		{
			// オブジェクトの境界範囲から登録モートン番号を算出
			DWORD Elem = GetMortonNumber(Min, Max);
			if (Elem < m_dwCellNum) {
				// 空間が無い場合は新規作成
				if (!ppCellAry[Elem])
					CreateNewCell(Elem);
				return ppCellAry[Elem]->Push(spOFT);
			}
			return false;	// 登録失敗
		}

		// 衝突判定リストを作成する
		void GetAllCollisionList(vector<T*>& ColVect)
		{
			// リスト（配列）は必ず初期化します
			ColVect.clear();

			// ルート空間の存在をチェック
			if (ppCellAry[0] == NULL)
				return;	// 空間が存在していない

							// ルート空間を処理
			list<T*> ColStac;
			GetCollisionList(0, ColVect, ColStac);
		}



	protected:
		// 空間内で衝突リストを作成する
		bool GetCollisionList(DWORD Elem, vector<T*>& ColVect, list<T*>& ColStac)
		{
			typename list<T*>::iterator it;
			// ① 空間内のオブジェクト同士の衝突リスト作成
			OBJECT_FOR_TREE<T>* spOFT1 = ppCellAry[Elem]->GetFirstObj();
			while (spOFT1 != NULL)
			{
				OBJECT_FOR_TREE<T>* spOFT2 = spOFT1->m_spNext;
				while (spOFT2 != NULL) {
					// 衝突リスト作成
					ColVect.push_back(spOFT1->m_pObject);
					ColVect.push_back(spOFT2->m_pObject);
					spOFT2 = spOFT2->m_spNext;
				}
				// ② 衝突スタックとの衝突リスト作成
				for (it = ColStac.begin(); it != ColStac.end(); it++) {
					ColVect.push_back(spOFT1->m_pObject);
					ColVect.push_back(*it);
				}
				spOFT1 = spOFT1->m_spNext;
			}

			bool ChildFlag = false;
			// ③ 子空間に移動
			DWORD ObjNum = 0;
			DWORD i, NextElem;
			for (i = 0; i < 8; i++) {
				NextElem = Elem * 8 + 1 + i;
				if (NextElem < m_dwCellNum && ppCellAry[Elem * 8 + 1 + i]) {
					if (!ChildFlag) {
						// ④ 登録オブジェクトをスタックに追加
						spOFT1 = ppCellAry[Elem]->GetFirstObj();
						while (spOFT1) {
							ColStac.push_back(spOFT1->m_pObject);
							ObjNum++;
							spOFT1 = spOFT1->m_spNext;
						}
					}
					ChildFlag = true;
					GetCollisionList(Elem * 8 + 1 + i, ColVect, ColStac);	// 子空間へ
				}
			}

			// ⑤ スタックからオブジェクトを外す
			if (ChildFlag) {
				for (i = 0; i < ObjNum; i++)
					ColStac.pop_back();
			}

			return true;
		}


		// 空間を生成
		bool CreateNewCell(DWORD Elem)
		{
			// 引数の要素番号
			while (!ppCellAry[Elem])
			{
				// 指定の要素番号に空間を新規作成
				ppCellAry[Elem] = NEW CCell<T>;

				// 親空間にジャンプ
				Elem = (Elem - 1) >> 3;
				if (Elem >= m_dwCellNum) break;
			}
			return true;
		}

		// 座標から空間番号を算出
		DWORD GetMortonNumber(VECTOR* Min, VECTOR* Max)
		{
			// 最小レベルにおける各軸位置を算出
			DWORD LT = GetPointElem(*Min);
			DWORD RB = GetPointElem(*Max);

			// 空間番号を引き算して
			// 最上位区切りから所属レベルを算出
			DWORD Def = RB ^ LT;
			unsigned int HiLevel = 1;
			unsigned int i;
			for (i = 0; i < m_uiLevel; i++)
			{
				DWORD Check = (Def >> (i * 3)) & 0x7;
				if (Check != 0)
					HiLevel = i + 1;
			}
			DWORD SpaceNum = RB >> (HiLevel * 3);
			DWORD AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 7;
			SpaceNum += AddNum;

			if (SpaceNum > m_dwCellNum)
				return 0xffffffff;

			return SpaceNum;
		}

		// ビット分割関数
		DWORD BitSeparateFor3D(BYTE n)
		{
			DWORD s = n;
			s = (s | s << 8) & 0x0000f00f;
			s = (s | s << 4) & 0x000c30c3;
			s = (s | s << 2) & 0x00249249;
			return s;
		}

		// 3Dモートン空間番号算出関数
		DWORD Get3DMortonNumber(BYTE x, BYTE y, BYTE z)
		{
			return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
		}

		// 座標→線形8分木要素番号変換関数
		DWORD GetPointElem(VECTOR& p)
		{
			return Get3DMortonNumber(
				(BYTE)((p.x - m_RgnMin.x) / m_Unit.x),
				(BYTE)((p.y - m_RgnMin.y) / m_Unit.y),
				(BYTE)((p.z - m_RgnMin.z) / m_Unit.z)
			);
		}
	};






	/////////////////////////////////////
	// 空間クラス
	//////////////////////////////////
	template <class T>
	class CCell
	{
	protected:
		OBJECT_FOR_TREE<T>* m_spLatest;	// 最新OFLへのスマートポインタ

	public:
		// コンストラクタ
		CCell()
		{
			m_spLatest = nullptr;
		}

		// デストラクタ
		virtual ~CCell()
		{
			if (m_spLatest != NULL)
				ResetLink(m_spLatest);

		}

		// リンクを全てリセットする
		void ResetLink(OBJECT_FOR_TREE<T>*& spOFT)
		{
			if (spOFT->m_spNext != NULL)
				ResetLink(spOFT->m_spNext);
			spOFT = (NULL);		// スマートポインタリセット		
		}

		// OFTをプッシュ
		bool Push(OBJECT_FOR_TREE<T>*& spOFT)
		{
			if (spOFT == NULL) return false;	// 無効オブジェクトは登録しない
			if (spOFT->m_pCell == this) return false;	// 2重登録チェック
			if (m_spLatest == NULL) {
				m_spLatest = spOFT;	// 空間に新規登録
			}
			else
			{
				// 最新OFTオブジェクトを更新
				spOFT->m_spNext = m_spLatest;
				m_spLatest->m_spPre = spOFT;
				m_spLatest = spOFT;
			}
			spOFT->RegistCell(this);	// 空間を登録
			return true;
		}

		OBJECT_FOR_TREE<T>*& GetFirstObj()
		{
			return m_spLatest;
		}

		// 削除されるオブジェクトをチェック
		bool OnRemove(OBJECT_FOR_TREE<T>* pRemoveObj)
		{
			if (m_spLatest == pRemoveObj) {
				// 次のオブジェクトに挿げ替え
				if (m_spLatest != NULL)
					m_spLatest = m_spLatest->GetNextObj();
			}
			return true;
		}
	};
}  // end namespace IKD