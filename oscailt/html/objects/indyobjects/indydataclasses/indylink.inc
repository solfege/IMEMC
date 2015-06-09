<?

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



/************************************************************************/

/* Class to represent a simple links with meta information.

/************************************************************************/

class indyLinkObject extends indyBasicObject

{

   var $display_options = array(

      "Short",

      "Long",

      "Short, No Title",

      "Long, No Title",

      "Icon",

      "Icon And Text"

   );



   var $container_settables = array(

      "title" => 'text',

      "linktext" => 'text',

   );



   function indyLinkObject ($a, $b)

   {

      $this->indyBasicObject($a, $b);

   }



   function getDefaultDisplayMode($depth)

   {

      return "Long";

   }







   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)

   {

      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);

      $cls_str = ($cls == "") ? "" : "class='$cls'";

       list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, false);



      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")

      {

         $title = $caller_settings['title'];

      }

      else

      {

         $title = $this->getMeta("linktitle");

      }

      if($display == "Short, No Title" or $display == "Long, No Title" or $title == "")

      {

         $title_str = "";

      }

      else

      {

         $title_str = "title='$title'";

      }

      $link_target = $this->getMeta('linkdestination');

      if(isRelativePath($link_target) && !isAbsoluteURL($link_target))

      {

         $link_target = '<? echo $pseudo_directory_offset; ?>'.$link_target;

      }

      $link_html = "<a href='$link_target' $cls_str $title_str>";

      $link_end = "</a>";



      if($caller_settings && is_array($caller_settings) && isset($caller_settings['linktext']) && $caller_settings['linktext'] != "")

      {

         $link_text = $caller_settings['linktext'];

      }

      elseif(($display == "Long, No Title" or $display == "Long") and ($this->getMeta('longlinktext') != ""))

      {

         $link_text = $this->getMeta('longlinktext');

      }

      elseif($this->getMeta('shortlinktext') != "")

      {

         $link_text = $this->getMeta('shortlinktext');

      }

      else

      {

         $link_text = $link_target;

      }



      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());





      if($display == "Icon" and $img != "")

      {

         $link_full = $link_html.$img.$link_end;

      }

      elseif($display == "Icon And Text" and $img != "")

      {

         $link_full = $link_html.$img.$link_text.$link_end;

      }

      else

      {

         $link_full = $link_html.$link_text.$link_end;

      }

      $ret = $this->getHeadHTML($cls_str, $wrap);

      $ret .= $link_full;

      $ret .= $this->getFootHTML($cls_str, $wrap);

      return $ret;

   }

}

?>