class DisplayState{

	enum menuState{
		HOME,
		BUS_ENTRY,
		WIFI_STATUS
	};

	#define N_NAV_STATES 4
	#define NAV_STATE_NONE 0
	#define NAV_STATE_SET_BUS 1
	#define NAV_STATE_TOGGLE_BT 2
	#define NAV_STATE_WIFI_STATUS 3

	// Max 10 characters
	const char *navStateStrings[N_NAV_STATES] = {"", "Set Bus ID", "Toggle BT", "Wi-Fi Stat"};

	menuState currentMenu = HOME;
	int currentNavOption = NAV_STATE_NONE;

	#define BUS_ID_SIZE 3
	char busEntryID[BUS_ID_SIZE+2] = "_  ";
	int busEntryPos = 0;

	int busID = 0;
	int battery = 100;
	bool btEnable = true;

	Adafruit_LiquidCrystal* lcd;

public:
 	DisplayState(Adafruit_LiquidCrystal* lc_lcd);
  	void init();
	void rotaryRight();
	void rotaryLeft();
	void rotarySelect();
	void keypadPress(char digit);
	void backspace();
	void back();
	void setBusID(const char* id);
	void toggleBT();

private:
	void navigateTo(menuState menuState);
	void render();
	void setDisplay(const char *string);

};
