#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <algorithm>
#include <vector>
#include <regex>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#define MIN_CLASS_WIDTH 30

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
std::string getHTML(std::string url);
std::string getBorderedString(std::string source, std::string start, std::string end);
std::string remove_A_href(std::string input);

int reallength(std::string source);


std::string draw(std::string layout);

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        strings.push_back(str.substr(start, end - start));
    }
    return strings;
}

class course
{
	public: 
	std::string time;
	std::string title;
	std::string description;
	std::string room;
	bool last;
	std::string display_string;

	course(std::string init)
	{
		last = false;
		time = remove_A_href(getBorderedString(init,"<div class=\"cal-time\">","</div>"));
		title = remove_A_href(getBorderedString(init,"<div class=\"cal-title\">","</div>"));
		description = remove_A_href(getBorderedString(init,"<div class=\"cal-text\">","</div>"));
		room = remove_A_href(getBorderedString(init,"<div class=\"cal-res\">","</div>"));

	}

	void generate()
	{

		
		display_string += draw(time + " " + title);
		if(description != time) display_string += draw(description);
		if(room != time) display_string += draw("   " + room);
		display_string += (last)?draw("╚═╝"):draw("╟─╢");

		//std::cout << display_string;
	}
};

int reallength(std::string input)
{
	input = std::regex_replace (input,std::regex("ö"),"o");
	input = std::regex_replace (input,std::regex("ä"),"a");
	input = std::regex_replace (input,std::regex("ü"),"u");

	input = std::regex_replace (input,std::regex("Ö"),"O");
	input = std::regex_replace (input,std::regex("Ä"),"A");
	input = std::regex_replace (input,std::regex("Ü"),"U");


	return input.length();

}

std::string remove_A_href(std::string input)
{
		std::replace(input.begin(), input.end(), '\n', ' ');
		//std::replace(input.begin(), input.end(), 'ö', 'o');
		//input = std::regex_replace (input,std::regex("ö"),"oe");
		input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
		std::replace(input.begin(), input.end(), '\t', ' ');


	return getBorderedString(input, ">", "<");
}

std::string getBorderedString(std::string source, std::string start, std::string end)
{
	auto start_pos = source.find(start);
	auto end_pos = source.find(end,start_pos);

	
	// source.substr(start_pos);
	return source.substr(start_pos + start.length(), end_pos - start_pos - start.length());
}

std::string draw(std::string layout)
{
	std::string res = "";
	if(layout == "╔═╗")
	{
		res += "╔";
		for(int i = 0; i < MIN_CLASS_WIDTH-2; i++)res += "═";
		res += "╗\n";
	}
	else if(layout == "╠═╣")
	{
		res += "╠";
		for(int i = 0; i < MIN_CLASS_WIDTH-2; i++)res += "═";
		res += "╣\n";
	}
	
	else if(layout == "╟─╢")
	{
		res += "╟";
		for(int i = 0; i < MIN_CLASS_WIDTH-2; i++)res += "─";
		res += "╢\n";
	}
	else if(layout == "╚═╝")
	{
		res += "╚";
		for(int i = 0; i < MIN_CLASS_WIDTH-2; i++)res += "═";
		res += "╝\n";
	}
	else if(layout.substr(0,3) == "   ")
	{
		layout = layout.substr(3);
		layout += " ";
		int textwidth = MIN_CLASS_WIDTH - 4;

		size_t pos;

		while(true)
		{
			for(pos = 0; layout.find(" ", pos +1) < textwidth && layout.find(" ", pos +1) != std::string::npos; pos = layout.find(" ", pos +1));
			if(pos == std::string::npos) pos = layout.length();
			if(layout.find(" ", pos +1) == std::string::npos) pos = layout.length();

			std::string line = layout.substr(0,pos);
			res += "║ ";
			for(int i = pos; i < textwidth; i++) res += " ";
			for(int i = 0; i < (line.length() - reallength(line)); i++) res += " ";

			res += line + " ║\n";
			if(layout.length() <= pos) break;
			layout = layout.substr(pos +1);

		}

	}
	else 
	{
		int textwidth = MIN_CLASS_WIDTH - 4;

		size_t pos;
		layout += " ";

		while(true)
		{
			for(pos = 0; layout.find(" ", pos +1) < textwidth && layout.find(" ", pos +1) != std::string::npos; pos = layout.find(" ", pos +1));
			if(pos == std::string::npos) pos = layout.length();
			if(layout.find(" ", pos +1) == std::string::npos) pos = layout.length();

			std::string line = layout.substr(0,pos);
			res += "║ " + line;
			for(int i = pos; i < textwidth; i++) res += " ";
			for(int i = 0; i < (line.length() - reallength(line)); i++) res += " ";

			res += (layout.length() <= pos)?" ║\n":" ║\n";


			if(layout.length() <= pos) break;
			layout = layout.substr(pos +1);

		}
	}

	return res;
}

class day
{
	public:
	std::string title;
	std::vector<course> classes;
	std::string display_string;

	std::vector<std::string> lines;

	day(std::string init)
	{
		title = getBorderedString(init,"<li data-role=\"list-divider\">","</li>");

		auto lastcourse = init.find("<li");
		if(lastcourse == std::string::npos) return;
		lastcourse = init.find("<li", lastcourse+1);
		if(lastcourse == std::string::npos) return;

		init = init.substr(lastcourse);
		//std::cout << init <<"\n\n";

		
		lastcourse = 0;
		for(size_t pos = lastcourse; lastcourse != std::string::npos; pos = init.find("<li", pos +1))
		{
			std::string classstring = init.substr(lastcourse, pos - lastcourse);

			if(classstring == "") continue;

			classes.push_back(course(classstring));
			lastcourse = pos;

		}		

		if(classes.size() > 0)
		classes[classes.size()-1].last = true;
	}

	void generate()
	{
		display_string += draw("╔═╗");
		display_string += draw(title);
		display_string +=draw("╠═╣");


		for(course& c : classes)
		{
			c.generate();

			display_string += c.display_string;
		}
		lines = split(display_string, '\n');

		//std::cout << display_string;


	
	}
};

int main (int argc, char **argv)
{
    int cols = 80;
    int lines = 24;

#ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    cols = ts.ts_cols;
    lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    cols = ts.ws_col;
    lines = ts.ws_row;
#endif /* TIOCGSIZE */

	setlocale(LC_ALL, "");
	curl_global_init(CURL_GLOBAL_ALL);	

    std::string url = "https://onlinevault.de";
	//url = "https://onlinevault.de/index.php?action=view&gid=3067001&uid=8065001&date=1635112800";

	auto html = getBorderedString(getHTML(url),"<div class=\"ui-grid-e\">","<div class=\"footer-txt-l\">");

	std::vector<day> days = std::vector<day>();
	size_t lastpos = html.find("<div class=\"ui-block-", 0);

	for(size_t pos = lastpos; lastpos != std::string::npos; pos = html.find("<div class=\"ui-block-", pos +1))
	{
		std::string daystring = html.substr(lastpos, pos - lastpos);
		if(daystring == "") continue;

		days.push_back(day(daystring));

		lastpos = pos;

	}

	for(day& d : days)
	{
		if(d.classes.size() > 0)
		{
			d.generate();


		}
	}

	int daycols = cols / (MIN_CLASS_WIDTH + 2);
	for(int row = 0; row < (days.size() / daycols) +1; row++)
	{


		int maxlines = 0;
		for(int col = 0; col < daycols; col++)
		{
			int index = row * daycols + col;

			if(index >= days.size()) break;

			maxlines = std::max((int)days[index].lines.size(), maxlines);
		}


		for(int line_index = 0; line_index < maxlines; line_index++)
		{
			std::string line = "";
			for(int col = 0; col < daycols; col++)
			{
				int index = row * daycols + col;
				if(index >= days.size()) break;

				if(days[index].lines.size() > line_index)
				{
					line += days[index].lines[line_index] + "  ";
				}
				else
				{
					for(int i = 0; i < MIN_CLASS_WIDTH +2; i++) line += " ";
				}
			}

			std::cout << line << "\n";

		}
	}

	

	
	//std::cout << html;


}



size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	((std::string*)userp)-> append ((char*)buffer,realsize);
	return realsize;
}

std::string getHTML(std::string url)
{
	CURLcode res;
	char* errbuf[CURL_ERROR_SIZE];

	std::string readBuffer;
	
	CURL* easyhandle=curl_easy_init();

	curl_easy_setopt(easyhandle, CURLOPT_ERRORBUFFER, errbuf);
	curl_easy_setopt(easyhandle, CURLOPT_URL,url.c_str() );
	curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);
	res = curl_easy_perform(easyhandle);
	if(res == CURLE_OK) 
	{
	    char *nurl = NULL;
	    //curl_easy_getinfo(easyhandle, CURLINFO_REDIRECT_URL, &url);
		
	    curl_easy_getinfo(easyhandle, CURLINFO_EFFECTIVE_URL, &nurl);
	}
	else
	{
		fprintf(stderr, "%s\n", curl_easy_strerror(res));
	}
	return readBuffer;
}



