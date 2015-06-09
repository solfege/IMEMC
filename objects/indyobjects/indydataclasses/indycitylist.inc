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
/* Class to represent a listing of the indymedia network
/* Output to a cache file when any of its elements is updated.
/************************************************************************/
require_once("objects/indysitelist.inc");

class indyCitylistObject extends indyBasicObject
{
   function indyCitylistObject ( $a, $b )
   {
      $this->indyBasicObject ($a, $b);
   }

   var $display_options = array(
      "Short Link",
      "Long Link",
      "Icon",
      "Icon And Title",
      "Drop-Down",
      "List",
      "Intro And List",
      "String",
      "Paragraph",
      "Intro And Paragraph",
      "Full Page"
   );

   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   function getDefaultDisplayMode($depth)
   {
      if($depth == FULL_DEPTH) $display = "Full Page";
      elseif($depth == SUMMARY_DEPTH) $display = "Intro And Paragraph";
      elseif($depth == LONG_MENU_DEPTH) $display = "List";
      elseif($depth == SHORT_MENU_DEPTH) $display = "Paragraph";
      elseif($depth == LONG_LIST_DEPTH) $display = "List";
      elseif($depth == SHORT_LIST_DEPTH) $display = "Drop-Down";
      else $display = "Short Link";
      return $display;
   }


   function getRegionEntryHTML(&$indy_site, $cls_str, $display)
   {
      $link_str = "<a $cls_str href='$indy_site->url'>".ucfirst(strtolower( $indy_site->name ))."</a> ";
      if($display == "List" or $display =="Intro And List")
      {
         $ret = $link_str."<BR />\n";
      }
      elseif($display == "Drop-Down")
      {
         $ret = "<OPTION value='$indy_site->url'>$indy_site->name</OPTION>";
      }
      else $ret = $link_str;
      return $ret;
   }

   function getRegionHTML ( &$indyregion, $cls_str, $display )
   {
      $r_name = "<B $cls_str>".ucfirst ( strtolower($indyregion->name) ).":</B>";
      if($display == "Full Page")
      {
         $region_head_html = "<H3 $cls_str>$r_name</H3>\n";
      }
      elseif($display == "List" or $display =="Intro And List")
      {
         $region_head_html = "$r_name<BR />\n";
      }
      elseif($display == "Drop-Down")
      {
         $region_head_html = "<OPTION style='font-weight: bold;' value='http://www.indymedia.org'>".ucwords ( strtolower($indyregion->name) )."</OPTION>\n";
      }
      else $region_head_html = $r_name." ";

      $ret = $region_head_html;
      if($display == "Full Page")
      {
         $ret .= "<P $cls_str>";
      }
      foreach($indyregion->sites as $indy_site)
      {
         $ret .= $this->getRegionEntryHTML($indy_site, $cls_str, $display);
      }
      if($display == "Full Page")
      {
         $ret .= "</P>";
      }
      return $ret;
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      //the title string is overwritable with the title special setting
      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")
      {
         $link_text = $caller_settings['title'];
      }
      elseif(($display == "Full Page" or $display == "Intro And Paragraph" or $display =="Long Link") and $this->getMeta('longlinktext') != "")
      {
         $link_text = htmlspecialchars($this->getMeta('longlinktext'));
      }
      else
      {
         $link_text = htmlspecialchars($this->getMeta('shortlinktext'));
      }

      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());
      $link_target = $this->getSelfLink();
      $link_html = "$h<a href='$link_target' $cls_str>";
      $link_end = "</a>$h_e";
      if($display == "Icon" and $img != "")
      {
         $link_full = $link_html.$img.$link_end;
      }
      elseif($display == "Icon And Text" and $img != "")
      {
         $link_full = $link_html.$img.$link_text.$link_end;
      }
      elseif($display == "Full Page")
      {
         $link_full = $h.$link_text.$h_e;
      }
      else
      {
         $link_full = $link_html.$link_text.$link_end;
      }

      $expiry_hours = $this->getMeta("omlexpiry");
      $expiry_secs = $expiry_hours * 60 * 60;
      if($expiry_secs <= 12*60*60) $expiry_secs = 12*60*60;

      $ret = $this->getHeadHTML($cls_str, $wrap);
      $ret .= $link_full;

      if(($display == "Intro And List" or $display == "Intro And Paragraph" or $display == "Full Page") and $this->getMeta('summary') != "")
      {
         $ret .= "<P $cls_str>".htmlspecialchars($this->getMeta('summary'))."</P>";
      }
      if($this->isFullerDisplayThan($display,"Icon And Title"))
      {
         if($display == "Drop-Down")
         {
            $ret .= "<FORM style='display: inline; margin: 0' name='indycityform' action=''><SELECT $cls_str ONCHANGE='location = this.options[this.selectedIndex].value;'>\n";

         }
         $indysitelist = new IndySiteList();

         if ($this->getMeta("convertfromutf8") == "on" || $this->getMeta("convertfromutf8") == "true")
             $indysitelist->convert_from_utf8 = true;

         $indysitelist->loadList($this->getMeta("omlfileurl"), $expiry_secs);
         $indysitelist->addGlobalRegion();
         foreach( $indysitelist->regions as $indyregion )
         {
            $ret.= $this->getRegionHTML( $indyregion, $cls_str, $display );
         }
         if($display == "Drop-Down")
         {
            $ret .= "</SELECT></FORM>\n";
         }
      }
      $ret .= $this->getFootHTML($cls_str, $wrap);
      return $ret;
   }


}
?>
