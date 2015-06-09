<?php

//icons for attachments and allowed file extensions for uploads
$iconArray = array();
$iconArray[".bigpdf"]="big_pdficon.gif";
$iconArray[".pdf"]="pdficon.gif";
$iconArray[".doc"]="wordicon.gif";
$iconArray[".torrent"]="bittorrent_icon_16.png";
$iconDescriptionArray = array();
$iconDescriptionArray[".pdf"]="PDF Document";
$iconDescriptionArray[".doc"]="Word Document";
$iconDescriptionArray[".torrent"]="BitTorrent File";
$fileExtensions = array();
$fileExtensions['image'] = "jpg jpe jpeg gif png";	// bmp
$fileExtensions['video'] = "mov mpg mpeg m4v avi wmv asf rm";
$fileExtensions['audio'] = "mp3 mp4 m4a aiff mid snd ram ogg spx";	// wav
$fileExtensions['misc'] = "pdf txt xls torrent smil doc";
//  .doc and .ppt files have been removed as  we don't want to encourage proprietary text formats
// If it can be published as .doc or .ppt, it can be converted to HTML
// I left 

// mime types used when determining mime type of local and remote files when generating feeds
$mimeTypes = array( 
     	  	'.jpg' => 'image/jpeg', 
     	  	'.jpeg' => 'image/jpeg', 
     	  	'.jpe' => 'image/jpeg',
       		'.gif' => 'image/gif', 
       		'.png' => 'image/png', 
       		'.bmp' => 'image/bmp', 
       		'.tiff' => 'image/tiff', 
       		'.psd' => 'image/x-photoshop', 
       		'.ico' => 'image/x-icon', 
       		
       		'.mov' => 'video/quicktime', 
       		'.qt' => 'video/quicktime',
       		'.mpeg' => 'video/mpeg', 
       		'.mpg' => 'video/mpeg', 
       		'.m4v' => 'video/mpeg',
       		'.avi' => 'video/x-msvideo', 
       		'.wmv' => 'video/x-msvideo', 
       		'.asf' => 'video/x-msvideo', 
       		
       		'.mp3' => 'audio/mpeg', 
       		'.mp4' => 'audio/mpeg', 
       		'.m4a' => 'audio/mpeg',
       		'.wav' => 'audio/x-wav', 
       		'.ogg' => 'application/ogg', 
       		'.aac' => 'audio/aac', 
       		'.mid' => 'audio/mid', 
       		'.aiff' => 'audio/x-aiff',
       		'.snd' => 'audio/basic',
       		'.ram' => 'audio/x-pn-realaudio',
       		'.ra' => 'audio/x-pn-realaudio',
       		'.m3u' => 'audio/x-mpegurl',
       		
       		
       		'.doc' => 'application/vnd.ms-word', 
       		'.xls' => 'application/vnd.ms-excel', 
       		'.ppt' => 'application/vnd.ms-powerpoint', 
       		'.pdf' => 'application/pdf', 
       		'.zip' => 'application/x-zip', 
       		'.txt' => 'text/plain', 
       		'.html' => 'text/html', 
       		'.xhtml' => 'text/xhtml', 
       		'.tar' => 'application/x-tar', 
       		'.smil' => 'application/smil', 
       		/* '.smil' => 'application/smil+xml', The above is the new recommended mime type to be used for SMIL according to W3C specs for SMIL 2.0 at http://www.w3.org/2004/06/EditedREC-SMIL20-errata#E04  */
       		'.torrent' => 'application/x-bittorrent');
?>
