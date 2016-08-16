#pragma once

namespace gitkit {

class Preferences
{
	public:
		static Preferences& instance();
		
		bool generateCSS();
	
	private:
		Preferences();
		
private:
	static Preferences s_preferences;
};
	
}