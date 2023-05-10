#include <string>

// see https://github.com/nginx/nginx/blob/master/conf/mime.types

std::string getMIMEType(std::string const & uri) {
	std::string extension;
	size_t pos = uri.rfind('.');
	extension = (pos == std::string::npos) ? "txt" : uri.substr(pos + 1);
	if ((extension == "html") || (extension == "htm") || (extension == "shtml"))
		return ("text/html");
	else if ((extension == "css"))
		return ("text/css");
	else if ((extension == "xml"))
		return ("text/xml");
	else if ((extension == "gif"))
		return ("image/gif");
	else if ((extension == "jpeg") || (extension == "jpg"))
		return ("image/jpeg");
	else if ((extension == "js"))
		return ("application/javascript");
	else if ((extension == "atom"))
		return ("application/atom+xml");
	else if ((extension == "rss"))
		return ("application/rss+xml");
	else if ((extension == "mml"))
		return ("text/mathml");
	else if ((extension == "txt"))
		return ("text/plain");
	else if ((extension == "jad"))
		return ("text/vnd.sun.j2me.app-descriptor");
	else if ((extension == "wml"))
		return ("text/vnd.wap.wml");
	else if ((extension == "htc"))
		return ("text/x-component");
	else if ((extension == "avif"))
		return ("image/avif");
	else if ((extension == "png"))
		return ("image/png");
	else if ((extension == "svg") || (extension == "svgz"))
		return ("image/svg+xml");
	else if ((extension == "tif") || (extension == "tiff"))
		return ("image/tiff");
	else if ((extension == "wbmp"))
		return ("image/vnd.wap.wbmp");
	else if ((extension == "webp"))
		return ("image/webp");
	else if ((extension == "ico"))
		return ("image/x-icon");
	else if ((extension == "jng"))
		return ("image/x-jng");
	else if ((extension == "bmp"))
		return ("image/x-ms-bmp");
	else if ((extension == "woff"))
		return ("font/woff");
	else if ((extension == "woff2"))
		return ("font/woff2");
	else if ((extension == "jar") || (extension == "war") ||
			 (extension == "ear"))
		return ("application/java-archive");
	else if ((extension == "json"))
		return ("application/json");
	else if ((extension == "hqx"))
		return ("application/mac-binhex40");
	else if ((extension == "doc"))
		return ("application/msword");
	else if ((extension == "pdf"))
		return ("application/pdf");
	else if ((extension == "ps") || (extension == "eps") || (extension == "ai"))
		return ("application/postscript");
	else if ((extension == "rtf"))
		return ("application/rtf");
	else if ((extension == "m3u8"))
		return ("application/vnd.apple.mpegurl");
	else if ((extension == "xls") || (extension == "xlt") ||
			 (extension == "xlm") || (extension == "xld") ||
			 (extension == "xla") || (extension == "xlc") ||
			 (extension == "xlw") || (extension == "xll"))
		return ("application/vnd.ms-excel");
	else if ((extension == "ppt") || (extension == "pps"))
		return ("application/vnd.ms-powerpoint");
	else if ((extension == "wmlc"))
		return ("application/vnd.wap.wmlc");
	else if ((extension == "kml"))
		return ("application/vnd.google-earth.kml+xml");
	else if ((extension == "kmz"))
		return ("application/vnd.google-earth.kmz");
	else if ((extension == "7z"))
		return ("application/x-7z-compressed");
	else if ((extension == "cco"))
		return ("application/x-cocoa");
	else if ((extension == "jardiff"))
		return ("application/x-java-archive-diff");
	else if ((extension == "jnlp"))
		return ("application/x-java-jnlp-file");
	else if ((extension == "run"))
		return ("application/x-makeself");
	else if ((extension == "pl") || (extension == "pm"))
		return ("application/x-perl");
	else if ((extension == "pdb") || (extension == "pqr") ||
			 (extension == "prc") || (extension == "pde"))
		return ("application/x-pilot");
	else if ((extension == "rar"))
		return ("application/x-rar-compressed");
	else if ((extension == "rpm"))
		return ("application/x-redhat-package-manager");
	else if ((extension == "sea"))
		return ("application/x-sea");
	else if ((extension == "swf"))
		return ("application/x-shockwave-flash");
	else if ((extension == "sit"))
		return ("application/x-stuffit");
	else if ((extension == "tcl") || (extension == "tk"))
		return ("application/x-tcl");
	else if ((extension == "der") || (extension == "pem") ||
			 (extension == "crt"))
		return ("application/x-x509-ca-cert");
	else if ((extension == "xpi"))
		return ("application/x-xpinstall");
	else if ((extension == "xhtml") || (extension == "xht"))
		return ("application/xhtml+xml");
	else if ((extension == "zip"))
		return ("application/zip");
	else if ((extension == "bin") || (extension == "exe") ||
			 (extension == "dll"))
		return ("application/octet-stream");
	else if ((extension == "deb"))
		return ("application/octet-stream");
	else if ((extension == "dmg"))
		return ("application/octet-stream");
	else if ((extension == "eot"))
		return ("application/octet-stream");
	else if ((extension == "img") || (extension == "iso"))
		return ("application/octet-stream");
	else if ((extension == "msi") || (extension == "msp") ||
			 (extension == "msm"))
		return ("application/octet-stream");
	else if ((extension == "mid") || (extension == "midi") ||
			 (extension == "kar"))
		return ("audio/midi");
	else if ((extension == "mp3"))
		return ("audio/mpeg");
	else if ((extension == "ogg"))
		return ("audio/ogg");
	else if ((extension == "m4a"))
		return ("audio/x-m4a");
	else if ((extension == "ra"))
		return ("audio/x-realaudio");
	else if ((extension == "3gpp") || (extension == "3gp"))
		return ("video/3gpp");
	else if ((extension == "ts"))
		return ("video/mp2t");
	else if ((extension == "mp4"))
		return ("video/mp4");
	else if ((extension == "mpeg") || (extension == "mpg"))
		return ("video/mpeg");
	else if ((extension == "mov"))
		return ("video/quicktime");
	else if ((extension == "webm"))
		return ("video/webm");
	else if ((extension == "flv"))
		return ("video/x-flv");
	else if ((extension == "m4v"))
		return ("video/x-m4v");
	else if ((extension == "mng"))
		return ("video/x-mng");
	else if ((extension == "asx") || (extension == "asf"))
		return ("video/x-ms-asf");
	else if ((extension == "wmv"))
		return ("video/x-ms-wmv");
	else if ((extension == "avi"))
		return ("video/x-msvideo");
	else
		return ("text/plain");
}
