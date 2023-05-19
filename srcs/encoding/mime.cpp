#include <map>
#include <string>
#include <algorithm>
#include <iostream>

// Stolen from : https://github.com/lasselukkari/MimeTypes/blob/master/MimeTypes.cpp#L70-L418

static std::map<std::string, std::string> mimeMap;
static std::map<std::string, std::string> extensionMap; // For reverse lookup
/**
 * @brief Quick and dirty way to initialize the mimeMap, maybe
 * there's a better way to do it
 * 
 * @return void
 */
void initMimeTypes() {
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;
	mimeMap["3gpp"] = "audio/3gpp";
	mimeMap["jpm"] = "video/jpm";
	mimeMap["mp3"] = "audio/mp3";
	mimeMap["rtf"] = "text/rtf";
	mimeMap["wav"] = "audio/wave";
	mimeMap["xml"] = "text/xml";
	mimeMap["3g2"] = "video/3gpp2";
	mimeMap["3gp"] = "video/3gpp";
	mimeMap["3gpp"] = "video/3gpp";
	mimeMap["ac"] = "application/pkix-attr-cert";
	mimeMap["adp"] = "audio/adpcm";
	mimeMap["ai"] = "application/postscript";
	mimeMap["apng"] = "image/apng";
	mimeMap["appcache"] = "text/cache-manifest";
	mimeMap["asc"] = "application/pgp-signature";
	mimeMap["atom"] = "application/atom+xml";
	mimeMap["atomcat"] = "application/atomcat+xml";
	mimeMap["atomsvc"] = "application/atomsvc+xml";
	mimeMap["au"] = "audio/basic";
	mimeMap["aw"] = "application/applixware";
	mimeMap["bdoc"] = "application/bdoc";
	mimeMap["bin"] = "application/octet-stream";
	mimeMap["bmp"] = "image/bmp";
	mimeMap["bpk"] = "application/octet-stream";
	mimeMap["buffer"] = "application/octet-stream";
	mimeMap["ccxml"] = "application/ccxml+xml";
	mimeMap["cdmia"] = "application/cdmi-capability";
	mimeMap["cdmic"] = "application/cdmi-container";
	mimeMap["cdmid"] = "application/cdmi-domain";
	mimeMap["cdmio"] = "application/cdmi-object";
	mimeMap["cdmiq"] = "application/cdmi-queue";
	mimeMap["cer"] = "application/pkix-cert";
	mimeMap["cgm"] = "image/cgm";
	mimeMap["class"] = "application/java-vm";
	mimeMap["coffee"] = "text/coffeescript";
	mimeMap["conf"] = "text/plain";
	mimeMap["cpt"] = "application/mac-compactpro";
	mimeMap["crl"] = "application/pkix-crl";
	mimeMap["css"] = "text/css";
	mimeMap["csv"] = "text/csv";
	mimeMap["cu"] = "application/cu-seeme";
	mimeMap["davmount"] = "application/davmount+xml";
	mimeMap["dbk"] = "application/docbook+xml";
	mimeMap["deb"] = "application/octet-stream";
	mimeMap["def"] = "text/plain";
	mimeMap["deploy"] = "application/octet-stream";
	mimeMap["disposition-notification"] = "message/disposition-notification";
	mimeMap["dist"] = "application/octet-stream";
	mimeMap["distz"] = "application/octet-stream";
	mimeMap["dll"] = "application/octet-stream";
	mimeMap["dmg"] = "application/octet-stream";
	mimeMap["dms"] = "application/octet-stream";
	mimeMap["doc"] = "application/msword";
	mimeMap["dot"] = "application/msword";
	mimeMap["drle"] = "image/dicom-rle";
	mimeMap["dssc"] = "application/dssc+der";
	mimeMap["dtd"] = "application/xml-dtd";
	mimeMap["dump"] = "application/octet-stream";
	mimeMap["ear"] = "application/java-archive";
	mimeMap["ecma"] = "application/ecmascript";
	mimeMap["elc"] = "application/octet-stream";
	mimeMap["emf"] = "image/emf";
	mimeMap["eml"] = "message/rfc822";
	mimeMap["emma"] = "application/emma+xml";
	mimeMap["eps"] = "application/postscript";
	mimeMap["epub"] = "application/epub+zip";
	mimeMap["es"] = "application/ecmascript";
	mimeMap["exe"] = "application/octet-stream";
	mimeMap["exi"] = "application/exi";
	mimeMap["exr"] = "image/aces";
	mimeMap["ez"] = "application/andrew-inset";
	mimeMap["fits"] = "image/fits";
	mimeMap["g3"] = "image/g3fax";
	mimeMap["gbr"] = "application/rpki-ghostbusters";
	mimeMap["geojson"] = "application/geo+json";
	mimeMap["gif"] = "image/gif";
	mimeMap["glb"] = "model/gltf-binary";
	mimeMap["gltf"] = "model/gltf+json";
	mimeMap["gml"] = "application/gml+xml";
	mimeMap["gpx"] = "application/gpx+xml";
	mimeMap["gram"] = "application/srgs";
	mimeMap["grxml"] = "application/srgs+xml";
	mimeMap["gxf"] = "application/gxf";
	mimeMap["gz"] = "application/gzip";
	mimeMap["h261"] = "video/h261";
	mimeMap["h263"] = "video/h263";
	mimeMap["h264"] = "video/h264";
	mimeMap["heic"] = "image/heic";
	mimeMap["heics"] = "image/heic-sequence";
	mimeMap["heif"] = "image/heif";
	mimeMap["heifs"] = "image/heif-sequence";
	mimeMap["hjson"] = "application/hjson";
	mimeMap["hlp"] = "application/winhlp";
	mimeMap["hqx"] = "application/mac-binhex40";
	mimeMap["htm"] = "text/html";
	mimeMap["html"] = "text/html";
	mimeMap["ics"] = "text/calendar";
	mimeMap["ico"] = "image/x-icon";
	mimeMap["ief"] = "image/ief";
	mimeMap["ifb"] = "text/calendar";
	mimeMap["iges"] = "model/iges";
	mimeMap["igs"] = "model/iges";
	mimeMap["img"] = "application/octet-stream";
	mimeMap["in"] = "text/plain";
	mimeMap["ini"] = "text/plain";
	mimeMap["ink"] = "application/inkml+xml";
	mimeMap["inkml"] = "application/inkml+xml";
	mimeMap["ipfix"] = "application/ipfix";
	mimeMap["iso"] = "application/octet-stream";
	mimeMap["jade"] = "text/jade";
	mimeMap["jar"] = "application/java-archive";
	mimeMap["jls"] = "image/jls";
	mimeMap["jp2"] = "image/jp2";
	mimeMap["jpe"] = "image/jpeg";
	mimeMap["jpeg"] = "image/jpeg";
	mimeMap["jpf"] = "image/jpx";
	mimeMap["jpg"] = "image/jpeg";
	mimeMap["jpg2"] = "image/jp2";
	mimeMap["jpgm"] = "video/jpm";
	mimeMap["jpgv"] = "video/jpeg";
	mimeMap["jpm"] = "image/jpm";
	mimeMap["jpx"] = "image/jpx";
	mimeMap["js"] = "application/javascript";
	mimeMap["json"] = "application/json";
	mimeMap["json5"] = "application/json5";
	mimeMap["jsonld"] = "application/ld+json";
	mimeMap["jsonml"] = "application/jsonml+json";
	mimeMap["jsx"] = "text/jsx";
	mimeMap["kar"] = "audio/midi";
	mimeMap["ktx"] = "image/ktx";
	mimeMap["less"] = "text/less";
	mimeMap["list"] = "text/plain";
	mimeMap["litcoffee"] = "text/coffeescript";
	mimeMap["log"] = "text/plain";
	mimeMap["lostxml"] = "application/lost+xml";
	mimeMap["lrf"] = "application/octet-stream";
	mimeMap["m1v"] = "video/mpeg";
	mimeMap["m21"] = "application/mp21";
	mimeMap["m2a"] = "audio/mpeg";
	mimeMap["m2v"] = "video/mpeg";
	mimeMap["m3a"] = "audio/mpeg";
	mimeMap["m4a"] = "audio/mp4";
	mimeMap["m4p"] = "application/mp4";
	mimeMap["ma"] = "application/mathematica";
	mimeMap["mads"] = "application/mads+xml";
	mimeMap["man"] = "text/troff";
	mimeMap["manifest"] = "text/cache-manifest";
	mimeMap["map"] = "application/json";
	mimeMap["mar"] = "application/octet-stream";
	mimeMap["markdown"] = "text/markdown";
	mimeMap["mathml"] = "application/mathml+xml";
	mimeMap["mb"] = "application/mathematica";
	mimeMap["mbox"] = "application/mbox";
	mimeMap["md"] = "text/markdown";
	mimeMap["me"] = "text/troff";
	mimeMap["mesh"] = "model/mesh";
	mimeMap["meta4"] = "application/metalink4+xml";
	mimeMap["metalink"] = "application/metalink+xml";
	mimeMap["mets"] = "application/mets+xml";
	mimeMap["mft"] = "application/rpki-manifest";
	mimeMap["mid"] = "audio/midi";
	mimeMap["midi"] = "audio/midi";
	mimeMap["mime"] = "message/rfc822";
	mimeMap["mj2"] = "video/mj2";
	mimeMap["mjp2"] = "video/mj2";
	mimeMap["mjs"] = "application/javascript";
	mimeMap["mml"] = "text/mathml";
	mimeMap["mods"] = "application/mods+xml";
	mimeMap["mov"] = "video/quicktime";
	mimeMap["mp2"] = "audio/mpeg";
	mimeMap["mp21"] = "application/mp21";
	mimeMap["mp2a"] = "audio/mpeg";
	mimeMap["mp3"] = "audio/mpeg";
	mimeMap["mp4"] = "video/mp4";
	mimeMap["mp4a"] = "audio/mp4";
	mimeMap["mp4s"] = "application/mp4";
	mimeMap["mp4v"] = "video/mp4";
	mimeMap["mpd"] = "application/dash+xml";
	mimeMap["mpe"] = "video/mpeg";
	mimeMap["mpeg"] = "video/mpeg";
	mimeMap["mpg"] = "video/mpeg";
	mimeMap["mpg4"] = "video/mp4";
	mimeMap["mpga"] = "audio/mpeg";
	mimeMap["mrc"] = "application/marc";
	mimeMap["mrcx"] = "application/marcxml+xml";
	mimeMap["ms"] = "text/troff";
	mimeMap["mscml"] = "application/mediaservercontrol+xml";
	mimeMap["msh"] = "model/mesh";
	mimeMap["msi"] = "application/octet-stream";
	mimeMap["msm"] = "application/octet-stream";
	mimeMap["msp"] = "application/octet-stream";
	mimeMap["mxf"] = "application/mxf";
	mimeMap["mxml"] = "application/xv+xml";
	mimeMap["n3"] = "text/n3";
	mimeMap["nb"] = "application/mathematica";
	mimeMap["oda"] = "application/oda";
	mimeMap["oga"] = "audio/ogg";
	mimeMap["ogg"] = "audio/ogg";
	mimeMap["ogv"] = "video/ogg";
	mimeMap["ogx"] = "application/ogg";
	mimeMap["omdoc"] = "application/omdoc+xml";
	mimeMap["onepkg"] = "application/onenote";
	mimeMap["onetmp"] = "application/onenote";
	mimeMap["onetoc"] = "application/onenote";
	mimeMap["onetoc2"] = "application/onenote";
	mimeMap["opf"] = "application/oebps-package+xml";
	mimeMap["otf"] = "font/otf";
	mimeMap["owl"] = "application/rdf+xml";
	mimeMap["oxps"] = "application/oxps";
	mimeMap["p10"] = "application/pkcs10";
	mimeMap["p7c"] = "application/pkcs7-mime";
	mimeMap["p7m"] = "application/pkcs7-mime";
	mimeMap["p7s"] = "application/pkcs7-signature";
	mimeMap["p8"] = "application/pkcs8";
	mimeMap["pdf"] = "application/pdf";
	mimeMap["pfr"] = "application/font-tdpfr";
	mimeMap["pgp"] = "application/pgp-encrypted";
	mimeMap["pkg"] = "application/octet-stream";
	mimeMap["pki"] = "application/pkixcmp";
	mimeMap["pkipath"] = "application/pkix-pkipath";
	mimeMap["pls"] = "application/pls+xml";
	mimeMap["png"] = "image/png";
	mimeMap["prf"] = "application/pics-rules";
	mimeMap["ps"] = "application/postscript";
	mimeMap["pskcxml"] = "application/pskc+xml";
	mimeMap["qt"] = "video/quicktime";
	mimeMap["raml"] = "application/raml+yaml";
	mimeMap["rdf"] = "application/rdf+xml";
	mimeMap["rif"] = "application/reginfo+xml";
	mimeMap["rl"] = "application/resource-lists+xml";
	mimeMap["rld"] = "application/resource-lists-diff+xml";
	mimeMap["rmi"] = "audio/midi";
	mimeMap["rnc"] = "application/relax-ng-compact-syntax";
	mimeMap["rng"] = "application/xml";
	mimeMap["roa"] = "application/rpki-roa";
	mimeMap["roff"] = "text/troff";
	mimeMap["rq"] = "application/sparql-query";
	mimeMap["rs"] = "application/rls-services+xml";
	mimeMap["rsd"] = "application/rsd+xml";
	mimeMap["rss"] = "application/rss+xml";
	mimeMap["rtf"] = "application/rtf";
	mimeMap["rtx"] = "text/richtext";
	mimeMap["s3m"] = "audio/s3m";
	mimeMap["sbml"] = "application/sbml+xml";
	mimeMap["scq"] = "application/scvp-cv-request";
	mimeMap["scs"] = "application/scvp-cv-response";
	mimeMap["sdp"] = "application/sdp";
	mimeMap["ser"] = "application/java-serialized-object";
	mimeMap["setpay"] = "application/set-payment-initiation";
	mimeMap["setreg"] = "application/set-registration-initiation";
	mimeMap["sgi"] = "image/sgi";
	mimeMap["sgm"] = "text/sgml";
	mimeMap["sgml"] = "text/sgml";
	mimeMap["shex"] = "text/shex";
	mimeMap["shf"] = "application/shf+xml";
	mimeMap["shtml"] = "text/html";
	mimeMap["sig"] = "application/pgp-signature";
	mimeMap["sil"] = "audio/silk";
	mimeMap["silo"] = "model/mesh";
	mimeMap["slim"] = "text/slim";
	mimeMap["slm"] = "text/slim";
	mimeMap["smi"] = "application/smil+xml";
	mimeMap["smil"] = "application/smil+xml";
	mimeMap["snd"] = "audio/basic";
	mimeMap["so"] = "application/octet-stream";
	mimeMap["spp"] = "application/scvp-vp-response";
	mimeMap["spq"] = "application/scvp-vp-request";
	mimeMap["spx"] = "audio/ogg";
	mimeMap["sru"] = "application/sru+xml";
	mimeMap["srx"] = "application/sparql-results+xml";
	mimeMap["ssdl"] = "application/ssdl+xml";
	mimeMap["ssml"] = "application/ssml+xml";
	mimeMap["stk"] = "application/hyperstudio";
	mimeMap["styl"] = "text/stylus";
	mimeMap["stylus"] = "text/stylus";
	mimeMap["svg"] = "image/svg+xml";
	mimeMap["svgz"] = "image/svg+xml";
	mimeMap["t"] = "text/troff";
	mimeMap["t38"] = "image/t38";
	mimeMap["tei"] = "application/tei+xml";
	mimeMap["teicorpus"] = "application/tei+xml";
	mimeMap["text"] = "text/plain";
	mimeMap["tfi"] = "application/thraud+xml";
	mimeMap["tfx"] = "image/tiff-fx";
	mimeMap["tif"] = "image/tiff";
	mimeMap["tiff"] = "image/tiff";
	mimeMap["tr"] = "text/troff";
	mimeMap["ts"] = "video/mp2t";
	mimeMap["tsd"] = "application/timestamped-data";
	mimeMap["tsv"] = "text/tab-separated-values";
	mimeMap["ttc"] = "font/collection";
	mimeMap["ttf"] = "font/ttf";
	mimeMap["ttl"] = "text/turtle";
	mimeMap["txt"] = "text/plain";
	mimeMap["u8dsn"] = "message/global-delivery-status";
	mimeMap["u8hdr"] = "message/global-headers";
	mimeMap["u8mdn"] = "message/global-disposition-notification";
	mimeMap["u8msg"] = "message/global";
	mimeMap["uri"] = "text/uri-list";
	mimeMap["uris"] = "text/uri-list";
	mimeMap["urls"] = "text/uri-list";
	mimeMap["vcard"] = "text/vcard";
	mimeMap["vrml"] = "model/vrml";
	mimeMap["vtt"] = "text/vtt";
	mimeMap["vxml"] = "application/voicexml+xml";
	mimeMap["war"] = "application/java-archive";
	mimeMap["wasm"] = "application/wasm";
	mimeMap["wav"] = "audio/wav";
	mimeMap["weba"] = "audio/webm";
	mimeMap["webm"] = "video/webm";
	mimeMap["webmanifest"] = "application/manifest+json";
	mimeMap["webp"] = "image/webp";
	mimeMap["wgt"] = "application/widget";
	mimeMap["wmf"] = "image/wmf";
	mimeMap["woff"] = "font/woff";
	mimeMap["woff2"] = "font/woff2";
	mimeMap["wrl"] = "model/vrml";
	mimeMap["wsdl"] = "application/wsdl+xml";
	mimeMap["wspolicy"] = "application/wspolicy+xml";
	mimeMap["x3d"] = "model/x3d+xml";
	mimeMap["x3db"] = "model/x3d+binary";
	mimeMap["x3dbz"] = "model/x3d+binary";
	mimeMap["x3dv"] = "model/x3d+vrml";
	mimeMap["x3dvz"] = "model/x3d+vrml";
	mimeMap["x3dz"] = "model/x3d+xml";
	mimeMap["xaml"] = "application/xaml+xml";
	mimeMap["xdf"] = "application/xcap-diff+xml";
	mimeMap["xdssc"] = "application/dssc+xml";
	mimeMap["xenc"] = "application/xenc+xml";
	mimeMap["xer"] = "application/patch-ops-error+xml";
	mimeMap["xht"] = "application/xhtml+xml";
	mimeMap["xhtml"] = "application/xhtml+xml";
	mimeMap["xhvml"] = "application/xv+xml";
	mimeMap["xm"] = "audio/xm";
	mimeMap["xml"] = "application/xml";
	mimeMap["xop"] = "application/xop+xml";
	mimeMap["xpl"] = "application/xproc+xml";
	mimeMap["xsd"] = "application/xml";
	mimeMap["xsl"] = "application/xml";
	mimeMap["xslt"] = "application/xslt+xml";
	mimeMap["xspf"] = "application/xspf+xml";
	mimeMap["xvm"] = "application/xv+xml";
	mimeMap["xvml"] = "application/xv+xml";
	mimeMap["yaml"] = "text/yaml";
	mimeMap["yang"] = "application/yang";
	mimeMap["yin"] = "application/yin+xml";
	mimeMap["yml"] = "text/yaml";
	mimeMap["zip"] = "application/zip";

	// Loop through all mimeMap entries and add them to the extensionMap
	for (std::map<std::string, std::string>::iterator it = mimeMap.begin(); it != mimeMap.end(); ++it) {
		// Check if it already exists, if so, skip
		if (extensionMap.find(it->second) != extensionMap.end()) {
			continue;
		}
		extensionMap[it->second] = it->first;
	}

	// Override popular extensions
	extensionMap["application/octet-stream"] = "bin";
	extensionMap["application/javascript"] = "js";
	extensionMap["application/json"] = "json";
	extensionMap["application/xml"] = "xml";
	extensionMap["text/html"] = "html";
	extensionMap["text/plain"] = "txt";
	extensionMap["text/css"] = "css";
	extensionMap["image/jpeg"] = "jpg";
	extensionMap["image/png"] = "png";
	extensionMap["image/gif"] = "gif";
}

namespace webserv {
	namespace encoding {
		/**
		 * @brief Get mimetype from file extension
		 * 
		 * @param ext File extension
		 * @return std::string Mimetype
		 */
		std::string mimeFromExtension(const std::string &ext) {
			initMimeTypes();
			// Remove leading dot if present
			std::string extension = ext;
			if (extension[0] == '.') {
				extension = extension.substr(1);
			}

			// Lowercase
			std::transform(extension.begin(),
				extension.end(),
				extension.begin(),
				::tolower
			);

			// Find mime type (default to application/octet-stream for safety)
			std::string mime = "application/octet-stream";
			std::map<std::string, std::string>::iterator it = mimeMap.find(extension);
			if (it != mimeMap.end()) {
				mime = it->second;
			}
			return mime;
		}

		/**
		 * @brief Get mimetype from file path
		 * 
		 * @param path File path
		 * @return std::string Mimetype
		 */
		std::string mimeFromPath(const std::string &path) {
			initMimeTypes();
			// Get extension
			std::string extension = path.substr(path.find_last_of(".") + 1);
			return mimeFromExtension(extension);
		}

		/**
		 * @brief Get file extension from mimetype
		 * 
		 * @param mime Mimetype
		 * @return std::string File extension
		 */
		std::string extensionFromMime(const std::string &mime) {
			initMimeTypes();
			// Find extension (default to bin for safety)
			std::string extension = "bin";

			// Lowercase
			std::string mimeLower = mime;
			std::transform(mimeLower.begin(),
				mimeLower.end(),
				mimeLower.begin(),
				::tolower
			);
			std::map<std::string, std::string>::iterator it = extensionMap.find(mimeLower);
			if (it != extensionMap.end()) {
				extension = it->second;
			}
			return extension;
		}
	}
}
