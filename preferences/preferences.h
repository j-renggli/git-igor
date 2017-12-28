#pragma once

namespace gitigor {

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