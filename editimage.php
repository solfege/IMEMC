<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/
require_once("oscailt_init.inc");
require_once "objects/indyobjects/indydataobjects.inc";


$OSCAILT_SCRIPT = "editimage.php";
addToPageTitle("Manage and View Images");

function writeLocalAdminHeader($page_default, $current_page)
{
   global $OSCAILT_SCRIPT;

   if ( isset($_REQUEST['thumbnails']) && $_REQUEST['thumbnails'] == 'true') {
       $swap_mode ="Hide Thumbnails";
       $switch_mode ="thumbnails=false";
       $show_host_name ="&thumbnails=true";
   } else  {
       $swap_mode ="Show Thumbnails";
       $switch_mode ="thumbnails=true";
       $show_host_name ="&thumbnails=false";
   }
   $switch_mode .= "&page=".$current_page."&target_image=".$page_default;

   ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'>
	<a href="<?=$OSCAILT_SCRIPT?>?<?=$switch_mode?>"><?=$swap_mode?></a> 
	</TD></TR>
     </TABLE>
   <?
}

function writeImageList()
{
    global $OSCAILT_SCRIPT, $system_config, $pseudo_directory_offset;
    // Assign usefull values for the third index.
    $Image_types = array(1 => 'GIF', 2 => 'JPG', 3 => 'PNG', 4 => 'SWF', 5 => 'PSD', 6 => 'BMP',
                         7 => 'TIFF(intel byte order)', 8 => 'TIFF(motorola byte order)', 9 => 'JPC',
                         10 => 'JP2', 11 => 'JPX', 12 => 'JB2', 13 => 'SWC', 14 => 'IFF', 15 => 'WBMP',16 => 'XBM');

    if (isset($_REQUEST['target_image']) && $_REQUEST['target_image'] != "") {
        $imageList = array();
        $_REQUEST['target_image'] = trim($_REQUEST['target_image']);

	// See if it is a directory or something like name* -i.e last char is *
        if (is_dir($_REQUEST['target_image']) || substr($_REQUEST['target_image'],-1) == '*') {
            $imageList = readImageDir($_REQUEST['target_image'], true);
	} else {
            $imageList[] = $_REQUEST['target_image'];
	}
	$page_default = $_REQUEST['target_image'];
    } else {
        $subfolder =  strtolower(date("MY", mktime()))."/";
        $imageList = readImageDir($subfolder);
	$page_default =  $system_config->attachment_store . $subfolder;
    }

    $start_page = 1;
    if (isset($_REQUEST['page']) && $_REQUEST['page'] != "") $start_page = $_REQUEST['page'];
    $filesCount = count($imageList);
    $show_page_links = true;
    //$page_size = 100;
    $page_size = 50;
    $num_pages = round($filesCount / $page_size);

    if ($start_page >= $num_pages) $start_page = $num_pages;

    if ($filesCount < $page_size ) {
        $page_size = $filesCount;
        $show_page_links = false;
    }

    if ($start_page > 1 ) $start_index = $page_size * ($start_page -1);
    else $start_index = 0;

    // Hack for last page to reduce page size which is used for loop count to remaining fraction
    if ($start_page >= $num_pages) $page_size = $filesCount - $start_index;

    writeLocalAdminHeader($page_default, $start_page);
    if ($show_page_links == true ) {
        $top_link = "<a class='editor-option' href=".$OSCAILT_SCRIPT. "?page=1&target_image=".$page_default.">&lt;&lt;</a> ";
        $end_link = " <a class='editor-option' href=".$OSCAILT_SCRIPT. "?page=".$num_pages."&target_image=".$page_default.">&gt;&gt;</a>";

        if ($start_page == 1 ) {
            $prev_link = "";
            $next_link = "<a class='editor-option' href=".$OSCAILT_SCRIPT. "?page=".($start_page+1)."&target_image=".$page_default.">Next</a>" . $end_link;
        } else {
            $prev_link = $top_link . "<a class='editor-option' href=".$OSCAILT_SCRIPT. "?page=".($start_page-1)."&target_image=".$page_default.">Previous</a>";
            $next_link = "<a class='editor-option' href=".$OSCAILT_SCRIPT. "?page=".($start_page+1)."&target_image=".$page_default.">Next</a>" . $end_link;
        }
        if ($start_page >= $num_pages ) $next_link = "";
    }

    ?>
    </div>
    <br>
    <table align=center border=0 width="95%">
    <tr class=admin>
    <?
    if ($show_page_links == true) {
        ?>
        <th class=admin><?=$prev_link?></th>
        <th class=admin colspan=6>Images - Total Number of Files = <?=count($imageList)?> &nbsp;- Displaying Page <?=$start_page?> of <?=$num_pages?></th>
        <th class=admin><?=$next_link?></th>
        <?
    } else {
        ?> <th class=admin colspan=8>Images - Total Number of Files = <?=count($imageList)?></th> <?
    }

    ?>
    </tr>
    <tr class=admin>
        <th class=admin>&nbsp;#&nbsp;</th>
        <th class=admin>&nbsp;Path &amp; Name&nbsp;</th>
        <th class=admin>&nbsp;Size&nbsp;</th>
        <th class=admin>&nbsp;Type&nbsp;</th>
        <th class=admin>&nbsp;Dimensions&nbsp;</th>
        <th class=admin>&nbsp;Mime Type&nbsp;</th>
        <th class=admin>&nbsp;Day&nbsp;</th>
        <th class=admin>&nbsp;Details&nbsp;</th>
    </tr>
    <?
    $resize_factoring = array();
    $show_thumbnails = false;
    if (isset($_REQUEST['thumbnails']) && $_REQUEST['thumbnails'] == "true") $show_thumbnails = true;

    $show_bigImage = false;
    if (isset($_REQUEST['show_image']) && $_REQUEST['show_image'] == "true") $show_bigImage = true;
    if (count($imageList) <=3 ) $show_bigImage = true;

    $imageState = array();
    for($i=0;$i < $page_size; $i++)
    {
        $image = $imageList[$i+$start_index];
	// Really long filenames make the display real wide so make them small.
	if (strlen($image) > 72 ) $image_str = "<small>".$image."</small>";
	else $image_str = $image;

        if (file_exists($image) ) {
           $pathinfo=pathinfo($image);
           $image_type = strtolower($pathinfo["extension"]);
           $size_info = @getimagesize($image);

	   // It's not an image...
	   if ($size_info === false ) {
               ?>
               <tr class=admin>
                   <td class=admin align=left>&nbsp;<?=$i+$start_index+1?>&nbsp;</td>
                   <td class=admin>&nbsp;<?=$image_str?>&nbsp;</td>
                   <td class=admin align=center><?=floor(filesize($image)/1024)?>k</td>
                   <td class=admin align=left colspan=3> Attachment not an image </td>
                   <td class=admin align=center><?=date("D M d",filemtime($image))?></td>
                   <td class=admin align=center><a href="editimage.php?subpage=edit&other=<?=$imageList[$i+$start_index]?>"><img src='graphics/edit.gif' border=0></a></td>
               </tr>
               <?
               $imageState[] = false;
               $resize_factoring[] = 0;
               continue;
           }
           $imageState[] = true;

           $image_size = floor(filesize($image)/1024) . " k";

	   if ($size_info[0] > 800 ) $resize_factoring[] = 600;
           else $resize_factoring[] = 0;
           $image_dims = "width " . $size_info[0] . " height " . $size_info[1];
           $image_desc = image_type_to_mime_type($size_info[2]);

	   // Check if it is a SWF, PSD, BMP etc -see list above in Images_types array.
	   if ($size_info[2] == 5 || $size_info[2] == 5 || $size_info[2] >= 6) {
             $image_desc = "<b>" . $image_desc . "<b>";
           }

           ?>
           <tr class=admin>
               <td class=admin align=left>&nbsp;<?=$i+$start_index+1?>&nbsp;</td>
               <td class=admin><?=$image_str?></td>
               <td class=admin align=center><?=$image_size?></td>
               <td class=admin align=center><?=$image_type?></td>
               <td class=admin align=center><?=$image_dims?></td>
               <td class=admin align=center><?=$image_desc?></td>
               <td class=admin align=center><?=date("M d D H:i",filemtime($image))?></td>
               <td class=admin align=center><a href="editimage.php?subpage=edit&image=<?=$imageList[$i+$start_index]?>"> <img src='graphics/edit.gif' border=0> </a></td>
           </tr>
           <?
        } else {
           $resize_factoring[] = 0;
           ?>
           <tr class=admin>
               <td class=admin align=left>&nbsp;<?=$i+$start_index+1?>&nbsp;</td>
               <td class=admin>&nbsp;<?=$image_str?>&nbsp;</td>
               <td class=admin colspan =7 align=center> Attachment file not found </td>
           </tr>
           <?
        }
    }
    // Display the thumbbnail images and the image itself if not too big
    // echo("max execution time ". ini_get('max_execution_time'). "<BR>");
    if (count($imageList) > 50 && $show_thumbnails == true) ini_set('max_execution_time',120);

    if ($show_thumbnails == true || $show_bigImage == true) {
        for($i=0;$i < $page_size ;$i++)
        {
            $image = $imageList[$i+$start_index];
            $thumbnail_url = "";
            $dimension_str = "";
            if (file_exists($image) && $imageState[$i] == true) {
                $tool = new ImageTool();
   	        if (is_object($tool) ) {
                    $tool->show_error = "No image found";
   
                    $transformedImage_info=$tool->getTransformedImageURL($image, $system_config->story_summary_thumbnail);
                    if($transformedImage_info != null) {
                        $transformedImageURL = $transformedImage_info[0];
                        if(isRelativePath($transformedImageURL)) {
                            $transformedImageURL = $pseudo_directory_offset.$transformedImageURL;
                        }
                        $dimension_str = $transformedImage_info[2];
                        $thumbnail_url = $transformedImageURL;
                    }
                }
   
   	        if ($resize_factoring[$i] > 0 ) $dimension_str = "width='600' height='500'";
   
                if ($show_thumbnails == true) {
                    ?>
                    <tr class=admin>
                     <td class=admin align=left>&nbsp;<?=$i+$start_index+1?>&nbsp;</td>
                     <td class=admin colspan=7><?=$thumbnail_url?><br> <a href="editimage.php?subpage=edit&image=<?=$imageList[$i+$start_index]?>"><img src="<?=$thumbnail_url?>" /></a></td>
                    </tr>
                    <?
                }
                if ($show_bigImage == true) {
                    ?>
                    <tr class=admin>
                     <td class=admin align=left>&nbsp;<?=$i+$start_index+1?>&nbsp;</td>
                      <td class=admin colspan=7><br> <img src="<?=$image?>" <?=$dimension_str?> /></td>
                    </tr>
                    <?
                }
            }
        }
    }
    if (count($imageList) == 0)
    {
        ?>
        <tr class=admin>
            <td class=admin align=left>&nbsp;</td>
            <td class=admin colspan=7 align=center>No attachment/image files found for match <?=$_REQUEST['target_image']?></td>
        </tr>
        <?
    }
  
    ?>
    <tr class='admin'>
        <td colspan=8 align=center>
        <form action="editimage.php" method=post>
        <br />
        <input type=text maxlength=120 size=80 name='target_image' value="<?=$page_default?>">
	<p>
        <input type=submit name='image_test' value="Enter path to attachment directory or to an particular image to display info on">
        </form>
        </td>
    </tr>
    </table>
    <?
}
// Return a list of files for a given directory path. Accepts *
function readImageDir($sub_dir, $basedir_set=false)
{
      global $system_config, $path_prefix;

      $fileList = array();
      $search_pattern = false;

      if ($basedir_set == false) {
          $target_dir = $system_config->attachment_store . $sub_dir;
      } else {
          // Search for a * on the end indicating a pattern match and if present then remove it.
          if (substr($sub_dir,-1) == "*") {
              $sub_dir = substr($sub_dir,0,-1);
              // This is from the last occurence if any of '/' onwards.
              $pos_match = strrpos($sub_dir,'/');
    	      if ($pos_match === false) $search_pattern = false;
	      else {
	          // If '/' is the last character then there is no search really. Pos is zero based
	          if ($pos_match < (strlen($sub_dir)-1)) {
                      $base_match = substr($sub_dir,$pos_match+1);
                      $sub_dir = substr($sub_dir,0,$pos_match);
                      $search_pattern = true;
                   }
              }
          }
          $target_dir = $sub_dir; 
      }

      if (substr($target_dir,-1) != "/") $target_dir .= "/";

      if(!is_dir($path_prefix.$target_dir)) {
          writeError("Couldn't open image attachments directory: ".$target_dir);
          $_REQUEST['target_image'] = $target_dir;
          return null;
      }

      $dh=opendir($path_prefix.$target_dir) or reportError("Couldn't open image attachments directory: ".$target_dir);
      if ($dh == false) return null;

      while($t_file=readdir($dh))
      {
         // echo("Is DIR: ".  $target_dir.$t_file. "<BR>");
         if(!is_dir($target_dir.$t_file))
         {
            if ($search_pattern == true) {
                // echo("Matching: ".$base_match. " vs " .$t_file ."<BR>");
                if (strpos($t_file,$base_match) === false) continue;
            }
            $fileList[] = $target_dir.$t_file;
         }
      }
      closedir($dh);

      return $fileList;
}

function getCachedImageFullPath($image, $t_type = 1)
{
    global $system_config, $pseudo_directory_offset;

    switch ($t_type) {
        case 1:
           $ThumbnailTemplate = $system_config->story_summary_thumbnail;
	   break;

        case 2:
           $ThumbnailTemplate = $system_config->story_headline_thumbnail;
	   break;

        case 3:
           $ThumbnailTemplate = $system_config->newswire_bar_thumbnail;
	   break;

        case 4:
           $ThumbnailTemplate = $system_config->rss_bar_thumbnail;
	   break;

        default:
           $ThumbnailTemplate = $system_config->story_summary_thumbnail;
	   break;
    }

    $thumbnail_url = "";
    if (file_exists($image) ) {
        $tool = new ImageTool();
	if (is_object($tool) ) {
              $tool->show_error = "No image found";

              $transformedImage_info=$tool->getTransformedImageURL($image, $ThumbnailTemplate);
              if($transformedImage_info != null)
              {
                 $transformedImageURL = $transformedImage_info[0];
                 if(isRelativePath($transformedImageURL))
                 {
                    $transformedImageURL = $pseudo_directory_offset.$transformedImageURL;
                 }
                 $dimension_str = $transformedImage_info[2];
                 $thumbnail_url = $transformedImageURL;
              }
        }
    }
    return $thumbnail_url;
}
function writeUserMsg($user_msg)
{
    // Use the indyObject to handle messages.
    echo "<p class='user-message'>".$user_msg."</p>\n";
}
function writeError($error)
{
    ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function forceThumbnailsGeneration($targetImageFile)
{
    // This call in fact deletes the thumbnail even though this call actually creates it but next
    // time an attempt is made to reference it, it is created.
    // For article thumbail it is defined in the Article Object.
    // 1. Story Summary
    $cached_file = getCachedImageFullPath($targetImageFile, 1);
    if ($cached_file != "" ) unlink($cache_file);
    // 2. Story Headline
    $cached_file = getCachedImageFullPath($targetImageFile, 2);
    if ($cached_file != "" ) unlink($cache_file);
    // 3. Newswire Bar
    $cached_file = getCachedImageFullPath($targetImageFile, 3);
    if ($cached_file != "" ) unlink($cache_file);
    // 4. RSS Bar 
    $cached_file = getCachedImageFullPath($targetImageFile, 4);
    if ($cached_file != "" ) unlink($cache_file);
} 

// This function returns a string containing a list with URLs to the other attachments in the input
// story or comment which is the parent of the input attachment id.
function getOtherAttachments($attachment_id, $parent_id, $from_story = true)
{
    global $system_config, $pseudo_directory_offset, $prefix, $dbconn;
    global $OSCAILT_SCRIPT;
    // Get list of attachments for this comment and parent story.
    $related_content_urls = "Other attachments in this ";
    if ($from_story == true ) {
            $related_content_urls .= "story";
	    $result = sql_query("SELECT attachment_id,attachment_file,image,video,audio FROM ".$prefix."_attachments WHERE story_id=".$parent_id. " and comment_id=0 and attachment_id !=".$attachment_id, $dbconn, 2);
    } else {
            $related_content_urls .= "comment";
	    $result = sql_query("SELECT attachment_id,attachment_file,image,video,audio FROM ".$prefix."_attachments WHERE comment_id=".$parent_id. " and attachment_id !=".$attachment_id, $dbconn, 2);
    }

    checkForError($result);
    if(sql_num_rows( $result ) > 0) {

            $t_attachment_obj = new Attachment();
	    $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
	    $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

            for ($irow=0; $irow<sql_num_rows( $result ); $irow++)
            {
                list($t_attachment_obj->attachment_id,$t_attachment_obj->attachment_file,$t_attachment_obj->image, $t_attachment_obj->video, $t_attachment_obj->audio) = sql_fetch_row($result, $dbconn);
                $t_url = $url_base . $system_config->attachment_store . $t_attachment_obj->attachment_file.'">' .basename($t_attachment_obj->attachment_file).'</a>';
                $t_details = "";
		// Note: Neither embedded video or embedded audio exist as files. Must rules these out
		//       to determine if the type is misc.
		if ($t_attachment_obj->isImage() ) {
                    $t_type = " Image ";
                    //$t_details = " &nbsp; " . "<a href='http://localhost/my_php/oscailt-3.0/html/" . $OSCAILT_SCRIPT ."?subpage=edit&image=".$system_config->attachment_store . $t_attachment_obj->attachment_file."'>Click for details on " .$t_attachment_obj->attachment_file."</a>";
                    $t_details = ' &nbsp; ' . $url_base . $OSCAILT_SCRIPT .'?subpage=edit&image='.$system_config->attachment_store . $t_attachment_obj->attachment_file.'">Click for details on ' .$t_attachment_obj->attachment_file.'</a>';
                }
		else if ($t_attachment_obj->isVideo()) $t_type = " Video ";
		else if ($t_attachment_obj->isAudio()) $t_type = " Audio ";
		else if ($t_attachment_obj->isEmbeddedVideo()) $t_type = " Video ";
		else if ($t_attachment_obj->isEmbeddedAudio()) $t_type = " Video ";
		else $t_type = " Misc ";

                $related_content_urls .= "<BR>" . $t_type . " (".$t_attachment_obj->attachment_id.") " .$t_url . $t_details;
            }
    } else {
	    return "";
    }

    return $related_content_urls;
}

function splitFilename($filename)
{
    $pos = strrpos($filename, '.');
    if ($pos === false)
    { // dot is not found in the filename
        return array($filename, '',0); // no extension
    }
    else
    {
        $basename = substr($filename, 0, $pos);
        $extension = substr($filename, $pos+1);
        return array($basename, $extension, $pos);
    }
} 

// This function makes sure the input filename does not exist and if it does it appends an number to it.
// 
function confirmFilename($image_filename)
{
    $dup_n = 1;
    $duplicate_prefix = "";
    $pathparts = splitFilename($image_filename);
    $file_ext = $pathparts[1];

    while(file_exists($pathparts[0].$duplicate_prefix.$file_ext)) {
            $duplicate_prefix = "_".$dup_n;
            $dup_n++;
    }

    if ($dup_n > 1)
    	$image_filename = $pathparts[0].$duplicate_prefix.$file_ext;

    return $image_filename;
}
// This function handles the conversions through the request types settings.
// For the case of BMP to JPG we would want to rename the file and update the attachments table.
// For the other types, they are already displaying okay, so we just give them the same filename but new types.
// 
function convertImageType($image_filename, $convert_to)
{
    global $system_config;

    if ($convert_to != 1 && $convert_to != 3 && $convert_to != 6) return "";

    /*** delete cached image too ***/
    $cache_file = getCachedImageFullPath($image_filename);
    if ($cache_file != "") unlink($cache_file);

    // Convert from GIF to JPEG
    if ($convert_to == 1 ) {
        /*** There could be an issue with filetypes in upper case. ****/
	$new_filename = dirname($image_filename) . "/" .basename($image_filename,".gif") . ".jpg";
	$new_filename = confirmFilename($new_filename);
        /*** read in the GIF image ***/
        $img = Imagecreatefromgif($image_filename);
        $t_msg = "Convert from .gif to .jpg";

    } else if ($convert_to == 3 ) {
        // Convert from PNG to JPEG
        /*** read in the PNG image ***/
	$new_filename = dirname($image_filename) . "/" .basename($image_filename,".png") . ".jpg";
	$new_filename = confirmFilename($new_filename);
        $img = Imagecreatefrompng($image_filename);
        $t_msg = "Convert from .png to .jpg";

    } else if ($convert_to == 6 ) {
        // Convert from BMP to JPEG
        /*** delete cached image too ***/
        /*** read in the BMP image ***/
	$new_filename = dirname($image_filename) . "/" .basename($image_filename,".bmp") . ".jpg";
	$new_filename = confirmFilename($new_filename);

        if (!rename($image_filename, $new_filename) ) {
            echo("<b>Failure to rename existing file:  ".$image_filename . " to file: ". $new_filename ."</b><br>");
	    return "";
        }

        $img = ImageCreateFromBmp($new_filename);
        // Now update the attachment table if the attachment_id is set.
        if (isset($_REQUEST['attachment_id']) && $_REQUEST['attachment_id'] != "") {
	    $t_attachment = new Attachment();
	    $t_attachment->attachment_id = $_REQUEST['attachment_id'];
	    $t_attachment->attachment_file = str_replace($system_config->attachment_store,"",$new_filename);
	    $t_attachment->updateAttachmentFilename();
	}
        $t_msg = "Convert from .bmp to .jpg";
    } 
 

    echo("<b>Renamed existing file:  ".$image_filename . " to new file: ". $new_filename ."</b><br>");
    /*** write the new jpeg image ***/
    imagejpeg($img, $new_filename, 100);
    logAction("", $image_filename, "Image", $t_msg);

    return $new_filename;

}
function writeEditBox($is_from_form=false)
{
    global $system_config, $pseudo_directory_offset, $prefix, $dbconn;
    global $OSCAILT_SCRIPT;

    $is_image = false;
    if (isset($_REQUEST['image']) && $_REQUEST['image'] != "") {
        $image_filename = $_REQUEST['image'];
        $is_image = true;
    } elseif (isset($_REQUEST['other']) && $_REQUEST['other'] != "") {
        $image_filename = $_REQUEST['other'];
    } else {
       ?>
       <table align=center class=admin>
       <form action="editimage.php" method=post>
       <tr> <td colspan=4 align=center> No image filename passed to this display </td>
       </tr>
       <tr> <td colspan=4 align=center> <input type=submit name=cancel value="&lt;&lt; Cancel"> </td>
       </tr>
       </form>
       </table>
       <?
       return;
    }

    $t_full_len = strlen($image_filename);
    $t_base_len = strlen(basename($image_filename));
    $t_dir = substr($image_filename,0,($t_full_len-$t_base_len));
    $back_link = "&page=1&target_image=".$t_dir;
    $back_text = "Listing for " . $t_dir;
    ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'>
	<a href="<?=$OSCAILT_SCRIPT?>?<?=$back_link?>"><?=$back_text?></a> 
	</TD></TR>
     </TABLE>
    <?


    $rotateDegrees = 0;
    // If convert_right or convert_left exist then we are to rotate the image
    if (isset($_REQUEST['convert_right']) && $_REQUEST['convert_right'] != "") {
        $rotateDegrees = 90;
    } else if (isset($_REQUEST['convert_left']) && $_REQUEST['convert_left'] != "") {
        $rotateDegrees = 270;
    }

    // Rotate the image
    if ($rotateDegrees > 0 && $is_image == true) {
        /*** delete cached image too ***/
        $cache_file = getCachedImageFullPath($image_filename);
	if ($cache_file != "") unlink($cache_file);

        if (file_exists($image_filename) ) {
            $pathinfo=pathinfo($image_filename);
            $image_type = strtolower($pathinfo["extension"]);

            if($image_type=="jpg" or $image_type=='jpeg') $originalImage = @imagecreatefromjpeg($image_filename);
            else if($image_type=="gif") $originalImage = @imagecreatefromgif($image_filename);
            else if($image_type=="png") $originalImage = @imagecreatefrompng($image_filename);
            else $originalImage = null;
	    
            $backgroundColour = array(0xFF, 0xFF, 0xFF);
            $bgcolour = imagecolorallocate($originalImage, $backgroundColour[0], $backgroundColour[1], $backgroundColour[2]);
            $rotatedImage = imagerotate($originalImage, $rotateDegrees, $bgcolour);

	    $outputFile = dirname($image_filename) . "/" .basename($image_filename, ".".$image_type) ."_rot". ".jpg";

            $msg_str = "existing file: ".$image_filename . " to file: ". $outputFile;
            if (rename($image_filename, $outputFile) )
	        echo "<b>Renamed " . $msg_str ."</b><br>";
            else 
	        writeError("Failure to rename ".$msg_str);

            // echo("Existing file  ".$image_filename . " new file ". $outputFile ."<br>");
            touch($image_filename);
            If(!imagejpeg($rotatedImage,$image_filename,100))
                echo("Image rotation failed <BR>");

            imagedestroy($rotatedImage);

            //Read and write for owner(webserver), read for everybody else
            chmod($image_filename, 0644);
            // Log the fact that the image was rotated
            if ($rotateDegrees == 270 )
                logAction("", $image_filename, "Image", "Rotated -90 deg");
            else
                logAction("", $image_filename, "Image", "Rotated +90 deg");
        }
    }

    $update_cache = false;
    $rename_file  = false;
    if (isset($_REQUEST['rename_filetype']) && isset($_REQUEST['rename_targtype'])) $rename_file = true;
    if ($rename_file == true) $update_cache = true;

    // Convert from GIF to JPEG
    if (($update_cache == true) || (isset($_REQUEST['delete_cache']) && $_REQUEST['delete_cache'] != "" && $is_image == true)) {
        /*** delete cached image ***/
        $cache_file = getCachedImageFullPath($image_filename);
	if ($cache_file != "" ) {
	    if (file_exists($cache_file)) {
	        unlink($cache_file);
                echo("Cache thumbnail file:  ".$image_filename . " deleted. New cache file will be generated.<br>");
            }
        }
    }

    // Removed the leading attachment directory name

    $t_attachment = new Attachment();
    $t_attachment->attachment_file = str_replace($system_config->attachment_store,"",$image_filename);
    if ($rename_file == true) {
        $image_filename_orig = $image_filename;
        echo "Rename requested for file ".$image_filename." to type ".$_REQUEST['rename_targtype']."<BR>";

        $pathparts = splitFilename($image_filename);
	$file_ext = "." .$_REQUEST['rename_targtype'];
        $image_filename = $pathparts[0] . "." .$_REQUEST['rename_targtype'];

	$n=1;
        $duplicate_prefix = "";
        while(file_exists($pathparts[0].$duplicate_prefix.$file_ext)) {
            $duplicate_prefix = "_".$n;
            $n++;
        }
	$image_filename = $pathparts[0].$duplicate_prefix.$file_ext;

	if (rename($image_filename_orig, $image_filename) ) {
            echo("Renamed file:  ".$image_filename_orig. " to ". $image_filename."<br>");
            if ($t_attachment->loadByFileName() == true) {
                // Update attachment filename               
                $t_attachment->attachment_file = str_replace($system_config->attachment_store,"",$image_filename);
                $result = sql_query("UPDATE ".$prefix."_attachments SET attachment_file='".$t_attachment->attachment_file."' WHERE attachment_id=".$t_attachment->attachment_id, $dbconn, 2);
                checkForError($result);
                echo("Updated new filename to ".$t_attachment->attachment_file." in attachment table.<br>");
	   } else {
                echo("No entry for file ".$t_attachment->attachment_file." in attachment table for renaming.<br>");
           }
           logAction("", $image_filename_orig, "Image", "Rename filetype to ".$_REQUEST['rename_targtype']);
        } else {
           writeError("Failure to rename requested for file ".$image_filename_orig." to type ".$_REQUEST['rename_targtype']);
        }
    }

    // Convert from GIF to JPEG
    if (isset($_REQUEST['convert_1']) && $_REQUEST['convert_1'] != "" && $is_image == true) {
        convertImageType($image_filename, 1);
    } 
    // Convert from PNG to JPEG
    if (isset($_REQUEST['convert_3']) && $_REQUEST['convert_3'] != "" && $is_image == true) {
        convertImageType($image_filename, 3);
    } 
    // Convert from BMP to JPEG
    if (isset($_REQUEST['convert_6']) && $_REQUEST['convert_6'] != "" && $is_image == true) {
	// In this case the filename will have changed.
        $t_newfile = convertImageType($image_filename, 6);
	if ($t_newfile != "") {
	    $image_filename = $t_newfile;
	    $t_attachment->attachment_file = str_replace($system_config->attachment_store,"",$image_filename);
        }
    } 
   
      
    ?>
    <table align=center class=admin>
    <tr class=admin>
        <th class=admin>&nbsp;File Type&nbsp;</th>
        <th class=admin>&nbsp;Size&nbsp;</th>
        <th class=admin>&nbsp;Dimensions&nbsp;</th>
        <th class=admin>&nbsp;Mime Type&nbsp;</th>
        <th class=admin>&nbsp;Date-Time&nbsp;</th>
    </tr>
    <form action="editimage.php" method=post>
    <?

    $do_resize = false;
    $do_rotate = true;
    if (file_exists($image_filename) ) {
        $pathinfo=pathinfo($image_filename);

	$convert_str = "";
    	$convert_code = 0;
    	$camera_info = "";
    	$camera_msg = "";

	if ($is_image == true) {
            $image_type = strtolower($pathinfo["extension"]);
            $size_info = @getimagesize($image_filename);
            //$image_size = $size_info[0] . " x " . $size_info[1];
            $image_size = floor(filesize($image_filename)/1024) . " k";
            // $image_desc = "xyz";
            //$image_dims = $size_info[3];
    	    if ($size_info[0] > 600 ) $image_dims = "width='600' height='500'";
            else $image_dims = "width " . $size_info[0] . " height " . $size_info[1];
            $actual_image_dims = "width " . $size_info[0] . " height " . $size_info[1];
    
            $new_image_dims = "width " . (0.9 * $size_info[0] ) . " height " . round( 0.9 * $size_info[1] );

            $image_desc = image_type_to_mime_type($size_info[2]);

    	    // Check the values returned to see what type of image it is and generate options to allow
    	    // conversions between various formats.
    	    switch ($size_info[2]) {
                case 1:
                    // GIF
                    $convert_str = "Convert Image from GIF to JPG";
        	    $convert_code = 1;
        	    break;
        
                case 2:
                    // JPG -Maybe add option to convert to PNG sometime...
        	    $convert_code = 0;
                    if (function_exists("exif_read_data") && !isset($_REQUEST['no_exif_read'])) {
        	            $camera_info = exif_read_data($image_filename,"'EXIF'", true);
                    } else {
        	            $camera_msg = "PHP Function 'exif_read_data' not supported";
                    }
        	    break;
        
                case 3:
                    // PNG
                    $convert_str = "Convert Image from PNG to JPG";
        	    $convert_code = 3;
        	    break;
        
                case 4:
                case 5:
                    // SWF and PSD ??
        	    break;
        
                case 6:
                    // BMP
                    $convert_str = "Convert from BMP to JPG";
                    $image_desc = "<b>" . $image_desc . "<b>";
        	    $convert_code = 6;
        	    break;
        
                case 7:
                    // TIFF (intel byte order)
        	    break;
        
                case 8:
                    // TIFF (Motorola byte order)
        	    break;
        
                default:
                    // 9 = JPC, 10 = JP2, 11 = JPX, 12 = JB2, 13 = SWC, 14 = IFF. 
                    break;
            }
        } else {
            // Handle the case where it is not an image...
            $image_type = strtolower($pathinfo["extension"]);
            $image_size = floor(filesize($image_filename)/1024) . " k";
            $actual_image_dims = "N/A";
            $image_desc = " N/A ";
        }

        ?>
           <tr class=admin>
               <td class=admin align=center><?=$image_type?></td>
               <td class=admin align=center><?=$image_size?></td>
               <td class=admin align=center><?=$actual_image_dims?></td>
               <td class=admin align=center><?=$image_desc?></td>
               <td class=admin align=center><?=date("D M d Y H:i:s",filemtime($image_filename))?></td>
           </tr>
           <tr class=admin>
               <th class=admin>&nbsp;Additional Info&nbsp;</th>
               <th class=admin colspan=4>&nbsp;Values&nbsp;</th>
           </tr>
        <?

	// Find the story or comment used by this attachment.
        if ($t_attachment->loadByFileName() == true) {
            // Handling http and https 
	    $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
	    $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';
            $related_content_urls = "";

	    if ($t_attachment->comment_id > 0 ) {
                $content_type = "Comment";
                $ref_url = $url_base . 'article/' . $t_attachment->story_id. '#comment'. $t_attachment->comment_id .'">Comment ' . $t_attachment->comment_id .'</a>';
                $story_comment_str = " (story ".$t_attachment->story_id.")";
                // Get the comment title:
		$result = sql_query("SELECT comment_title,hidden FROM ".$prefix."_comments WHERE comment_id=".$t_attachment->comment_id, $dbconn, 2);
                checkForError($result);
		$ref_title = "";
                if(sql_num_rows( $result ) > 0) {
                    list($ref_title,$content_status) = sql_fetch_row($result, $dbconn);
                }

                $related_content_urls = getOtherAttachments($t_attachment->attachment_id, $t_attachment->comment_id, false);
            } else {
		    // Make sure to continue using single quotes
                $content_type = "Story";
                $ref_url = $url_base . 'article/' . $t_attachment->story_id . '">Story Id ' .$t_attachment->story_id.'</a>';
                $story_comment_str = "";
                // Get the story title:
		$result = sql_query("SELECT story_title,hidden FROM ".$prefix."_stories WHERE story_id=".$t_attachment->story_id, $dbconn, 2);
                checkForError($result);
		$ref_title = "";
                if(sql_num_rows( $result ) > 0) {
                    list($ref_title,$content_status) = sql_fetch_row($result, $dbconn);
                }
                $related_content_urls=getOtherAttachments($t_attachment->attachment_id,$t_attachment->story_id,true);
            }
	    // Give a warning message if the width exceeds the allowed limit as this caused problems before.
	    if ($size_info[0] > $system_config->image_attachment_max_width) {
                ?>
                <tr class=admin>
                   <td class=admin align=left> <font class="error">Warning</font> </td>
                   <td class=admin align=left colspan=4><b>Image width <?=$size_info[0]?> is greater than configured max image width (<?=$system_config->image_attachment_max_width?>). This may leading to problems with thumbnails, resizing and rotations. </b></td> 
		</tr>
                <?
            }

	    if ($t_attachment->hidden == 1 ) $hide_state = "Hidden";
	    else $hide_state = "Visible";

	    if ($content_status == 1 ) $content_state = "Hidden";
	    else $content_state = "Visible";
           ?>
           <tr class=admin>
               <td class=admin align=left > Used by <?=$ref_url?> <?=$story_comment_str?></td>
               <td class=admin align=left colspan=5><?=$related_content_urls?></td> </tr>
           <tr class=admin>
               <td class=admin align=left > <?=$content_type?> Title</td>
               <td class=admin align=left colspan=5><?=$ref_title?></td>
           </tr>
           <tr class=admin>
               <td class=admin align=left > <strong>Attachment Id</strong> </td>
               <td class=admin align=left colspan=5><strong><?=$t_attachment->attachment_id?></strong>
               <input type=hidden name=attachment_id value="<?=$t_attachment->attachment_id?>">
	       </td>

           </tr>
           <tr class=admin>
               <td class=admin align=left > Image Status </td>
               <td class=admin align=left colspan=5><?=$hide_state?></td>
           </tr>
           <tr class=admin>
               <td class=admin align=left > <?=$content_type?> Status </td>
               <td class=admin align=left colspan=5><?=$content_state?></td>
           </tr>
           <tr class=admin>
               <td class=admin align=left > Image Description </td>
               <td class=admin align=left colspan=5><?=$t_attachment->description?></td>
           </tr>
           <tr class=admin>
               <td class=admin align=left > Time Posted </td>
               <td class=admin align=left colspan=5><?=date("F d Y H:i:s",$t_attachment->time_posted)?></td>
           </tr>
           <?
        } else {
	   ?>
           <tr class=admin>
               <td class=admin align=left > Article References</td>
               <td class=admin align=left colspan=5> No story or comment appears to reference this image</td>
           </tr>
           <?
        }

	if ($camera_info != "") {
           ?>
	   <tr class=admin>
               <td class=admin align=left  colspan=1> <b>Camera Info: </b> </td>
               <td class=admin align=left  colspan=4>
               <?
               // Removed Make and Model from list, so these should display now...
               $ignoreList = array("Software", "DateT","Compr", "JPEGInt","CCDWi", "Apert","Bright", "MaxAp","FlashP", "ColorS","Focal", "Sensing","Balance", "Focus","Version", "Sharp","Contrast", "Picture","SlowS", "Quality","Exposure", "Shutter");
               // exif_read_data returns an array of arrays which in this case is in camera_info variable.
               foreach ($camera_info as $key => $section )
               {
                   foreach ($section as $name => $c_val )
                   {
                       if ( in_array($name, $ignoreList)) {
                           continue;
                       } else  {
                           if (phpversion() >= 5 ) {
                               // Function only available in PHP 5
                               if (stripos($name, "MakerNote") === false) 
                                   echo($name . ": " . $c_val . "<BR>");
                           } else {
                               if (stristr($name, "MakerNote") === false) 
                                   echo($name . ": " . $c_val . "<BR>");
                           }
                       }
                   }
               }
               ?>
	       </td>
           </tr>
           <?
        }
	// Actually only called if exif_read_data is not available
	if ($camera_msg != "") {
           ?>
	   <tr class=admin>
               <td class=admin align=left  colspan=5>
	       <b>Camera Info: <b> <?=$camera_msg?> </td>
           </tr>
           <?
        }

	if ($convert_code == 1 || $convert_code == 3 || $convert_code == 6) {
           ?>
           <tr class=admin>
               <td class=admin align=left  colspan=1>
               <input type=submit name=convert_<?=$convert_code?> value="<?=$convert_str?>">
	       </td>
               <td class=admin align=left  colspan=4><i>
	       This will delete the cache image which will force an update.</i>
	       </td>
           </tr>
           <?
	} else {
           if (function_exists('exif_imagetype') ) {
              // Add more -see php manual
              $pic_types = array(1 => "gif", 2 => "jpg", 3 => "png", 4 => "swf", 5 => "psd", 6 => "bmp");
              $t_type = $image_type;
	      if ($t_type == "jpeg") $t_type = "jpg";

              $actual_type = exif_imagetype($image_filename);
	      if ($actual_type > 0 && $actual_type < 7 ) {
	          if (strcasecmp($pic_types[$actual_type], $t_type) != 0 ) {
                      ?>
                      <tr class=admin>
                          <td class=admin align=left colspan=1> <b>Consistency Check</b></td>
                          <td class=admin align=left colspan=4> Actual image type <b><?=$pic_types[$actual_type]?></b>
	                  does not match file type <b><?=$image_type?>. </b> &nbsp;
			  <input type=hidden name=rename_targtype value="<?=$pic_types[$actual_type]?>">
			  <input type=submit name=rename_filetype value="Rename file to correct filetype">
			  </td>
                      </tr>
                      <?
                  }
               }
           }
        }
	if ($do_resize == true) {
        ?>
           <tr class=admin>
               <td class=admin align=left  colspan=3>
               <input type=submit name=convert_2 value="Reduce size by 10%"> 
	       <br>This will delete the cache image which will force an update.
	       </td>
               <td class=admin align=left  colspan=2> New size would be: <?=$new_image_dims?> </td>
           </tr>
         <?
         }
	 if ($is_image == true && !isset($_REQUEST['no_thumbnail'])) {
         ?>
           <tr class=admin>
           <td class=admin colspan=5>Cache thumbnail: <?=getCachedImageFullPath($image_filename)?>
           <br> <img src="<?=getCachedImageFullPath($image_filename)?>" />
               <input type=submit name=delete_cache value="Rebuild cache image"> </td>
           </tr>
           <tr class=admin>
           <td class=admin colspan=5>Image: <?=$image_filename?> &nbsp; Displaying here as: <?=$image_dims?>
           <br> <a href="<?=$image_filename?>"><img src="<?=$image_filename?>" <?=$image_dims?> /></a></td>
           </tr>
         <?
         }
    } else {
         ?>
         <tr>
         <td colspan=4 align=center> No file found for: <?=$image_filename?> </td>
         </tr>
         <?
    }
    ?>
    <tr>
        <td colspan=4 align=center> &nbsp; </td>
    </tr>
    <?

    if ($is_image == true) {
    ?>
    <tr>
        <td colspan=5 align=center>
        <input type=hidden name=image value="<?=$image_filename?>">
        <input type=hidden name=subpage value="edit">
        <input type=submit name=convert_right value="Rotate +90 deg (Counter Clockwise)"> 
        <input type=submit name=convert_left value="Rotate -90 deg (Clockwise)"> 
        <input type=submit name=cancel value="Cancel">
	<br>Rotating will delete the cache image which will force an update.
        </td>
    </tr>
    <?
    }
    ?>
    </form>
    </table>
    <?
}

/***************************************
 *
 * @convert BMP to GD
 *
 * @param string $src
 *
 * @param string|bool $dest
 *
 * @return bool
 ****************************************
 */
function bmp2gd($src, $dest = false)
{
    /*** try to open the file for reading ***/
    if(!($src_f = fopen($src, "rb")))
    {
        return false;
    }

    /*** try to open the destination file for writing ***/ 
    if(!($dest_f = fopen($dest, "wb")))
    {
        return false;
    }

/*** grab the header ***/
$header = unpack("vtype/Vsize/v2reserved/Voffset", fread( $src_f, 14));

/*** grab the rest of the image ***/
$info = unpack("Vsize/Vwidth/Vheight/vplanes/vbits/Vcompression/Vimagesize/Vxres/Vyres/Vncolor/Vimportant",
fread($src_f, 40));

/*** extract the header and info into varibles ***/
extract($info);
extract($header);

/*** check for BMP signature ***/
if($type != 0x4D42)
{
    return false;
}

/*** set the pallete ***/
$palette_size = $offset - 54;
$ncolor = $palette_size / 4;
$gd_header = "";

/*** true-color vs. palette ***/
$gd_header .= ($palette_size == 0) ? "\xFF\xFE" : "\xFF\xFF";
$gd_header .= pack("n2", $width, $height);
$gd_header .= ($palette_size == 0) ? "\x01" : "\x00";
if($palette_size) {
$gd_header .= pack("n", $ncolor);
}
/*** we do not allow transparency ***/
$gd_header .= "\xFF\xFF\xFF\xFF";

/*** write the destination headers ***/
fwrite($dest_f, $gd_header);

/*** if we have a valid palette ***/
if($palette_size)
{
    /*** read the palette ***/
    $palette = fread($src_f, $palette_size);
    /*** begin the gd palette ***/
    $gd_palette = "";
    $j = 0;
    /*** loop of the palette ***/
    while($j < $palette_size)
    {
        $b = $palette{$j++};
        $g = $palette{$j++};
        $r = $palette{$j++};
        $a = $palette{$j++};
        /*** assemble the gd palette ***/
        $gd_palette .= "$r$g$b$a";
    }
    /*** finish the palette ***/
    $gd_palette .= str_repeat("\x00\x00\x00\x00", 256 - $ncolor);
    /*** write the gd palette ***/
    fwrite($dest_f, $gd_palette);
}

/*** scan line size and alignment ***/
$scan_line_size = (($bits * $width) + 7) >> 3;
$scan_line_align = ($scan_line_size & 0x03) ? 4 - ($scan_line_size & 0x03) : 0;

/*** this is where the work is done ***/
for($i = 0, $l = $height - 1; $i < $height; $i++, $l--)
{
    /*** create scan lines starting from bottom ***/
    fseek($src_f, $offset + (($scan_line_size + $scan_line_align) * $l));
    $scan_line = fread($src_f, $scan_line_size);
    if($bits == 24)
    {
        $gd_scan_line = "";
        $j = 0;
        while($j < $scan_line_size)
        {
            $b = $scan_line{$j++};
            $g = $scan_line{$j++};
            $r = $scan_line{$j++};
            $gd_scan_line .= "\x00$r$g$b";
        }
    }
    elseif($bits == 8)
    {
        $gd_scan_line = $scan_line;
    }
    elseif($bits == 4)
    {
        $gd_scan_line = "";
        $j = 0;
        while($j < $scan_line_size)
        {
            $byte = ord($scan_line{$j++});
            $p1 = chr($byte >> 4);
            $p2 = chr($byte & 0x0F);
            $gd_scan_line .= "$p1$p2";
        }
        $gd_scan_line = substr($gd_scan_line, 0, $width);
    }
    elseif($bits == 1)
    {
        $gd_scan_line = "";
        $j = 0;
        while($j < $scan_line_size)
        {
            $byte = ord($scan_line{$j++});
            $p1 = chr((int) (($byte & 0x80) != 0));
            $p2 = chr((int) (($byte & 0x40) != 0));
            $p3 = chr((int) (($byte & 0x20) != 0));
            $p4 = chr((int) (($byte & 0x10) != 0));
            $p5 = chr((int) (($byte & 0x08) != 0));
            $p6 = chr((int) (($byte & 0x04) != 0));
            $p7 = chr((int) (($byte & 0x02) != 0));
            $p8 = chr((int) (($byte & 0x01) != 0));
            $gd_scan_line .= "$p1$p2$p3$p4$p5$p6$p7$p8";
        }
    /*** put the gd scan lines together ***/
    $gd_scan_line = substr($gd_scan_line, 0, $width);
    }
    /*** write the gd scan lines ***/
    fwrite($dest_f, $gd_scan_line);
}
/*** close the source file ***/
fclose($src_f);
/*** close the destination file ***/
fclose($dest_f);

return true;
}

/*****************************************
 *
 * @ceate a BMP image
 *
 * @param string $filename
 *
 * @return bin string on success
 * @return bool false on failure
 *
 */
function ImageCreateFromBmp($filename)
{
    /*** create a temp file ***/
    $tmp_name = tempnam("/tmp", "GD");
    /*** convert to gd ***/
    if(bmp2gd($filename, $tmp_name))
    {
        /*** create new image ***/
        $img = imagecreatefromgd($tmp_name);
        /*** remove temp file ***/
        unlink($tmp_name);
        /*** return the image ***/
        return $img;
    }
    return false;
}



function writeConfirmDeleteBox()
{
    ?>

    <table align=center>
    <form action="editimage.php" method=post>
    <tr><td> Not implemented yet </td></tr>
    <input type=hidden name=subpage value="delete">

    <tr>
        <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete ....?</B><BR><BR></td>
    </tr>
    <tr>
        <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
        <td><input type=submit name=confirm value="Delete &gt;&gt;"></td>
    </tr>
    </form>
    </table>
    <?

}


ob_start();
$admin_table_width = "95%";

if($editor_session->isSessionOpen())

{
    writeAdminHeader("viewsitelog.php?log_type=action","View Logs");

    if($editor_session->editor->allowedReadAccessTo("editmonitor"))
    {
        if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']))
        {
            // Handle delete
            writeImageList();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage'] =="delete" && isset($_REQUEST['cancel']))
        {
            writeImageList();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
        {
            writeConfirmDeleteBox();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']))
        {
            // Handle edit ....
            writeEditBox(true);
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']))
        {
            writeEditBox();
        }
        else
        {
            writeImageList();
        }
    }
    else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();
include_once("adminfooter.inc");
?>