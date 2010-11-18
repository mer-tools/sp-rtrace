#include "terminal.h"
#include "options.h"


std::ostream &operator << (std::ostream &stream, const TerminalWriter& writer) {
	return writer.write(stream);
}


std::ostream& PngTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal png enhanced size " << Options::getInstance()->getScaleX() * 1024 / 100 <<
			"," << Options::getInstance()->getScaleY() * 768 / 100 << "\n";
	// font styling is not used, so the fontpath setting can be commented out
	//stream << "set fontpath \"/usr/share/fonts/truetype/ttf-liberation\"\n";
}


std::ostream& EpsTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal postscript enhanced color size " << Options::getInstance()->getScaleX() * 6 / 100 <<
			"," << Options::getInstance()->getScaleY() * 4 / 100 << "\n";
}


std::ostream& WxtTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal wxt enhanced size " << Options::getInstance()->getScaleX() * 1024 / 100 <<
			"," << Options::getInstance()->getScaleY() * 768 / 100 << "\n";
}


std::ostream& WxtTerminalFooter::write(std::ostream& stream) const {
	return stream <<"pause -1 \"The graph is displayed in other window. Hit enter when you are finished viewing it...\\n\"\n";
}
