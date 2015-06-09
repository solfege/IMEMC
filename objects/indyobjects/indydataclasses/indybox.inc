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

require_once( "indymenu.inc" );

/************************************************************************/
/* Class to represent a box (such as a context box).
/* Output to a cache file when any of its elements is updated.
/************************************************************************/
class indyBoxObject extends indyMenuObject
{
   var $display_options = array(
      "Box",
   );

   function indyBoxObject ( $a, $b )
   {
      $this->indyMenuObject ($a, $b);
   }

   function getDefaultDisplayMode($depth)
   {
      return "Box";
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      global $system_config;
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      $ret = $this->getHeadHTML($cls_str, true);
      $ret.= $this->getObjectEditingHTML($obj_set, "", "");
      if($this->getMeta("boxdisplay") == 'horizontal')
      {
         $ret .= "<table $cls_str><tr $cls_str>";
      }
      foreach( $this->containedItems['contained'] as $item )
      {
         if($this->getMeta("boxdisplay") == 'horizontal')
         {
            $ret .= "<td $cls_str>";
         }
         $s = $item->getAsCallerSettings();
         $cls_on = ($item->getStyle() == "") ? $cls : $item->getStyle();
         $cls_onstr = ($cls_on == "") ? "" : "class='$cls_on'";
         $needs_protection = false;
         if(substr($item->id, 0, 8) == 'content_')
         {
            $site_id = $obj_set->itemCollection[$this->objref]->site_id;
            if($item->isORProtected())
            {
               $needs_protection= true;
            }
            $site_id = $obj_set->itemCollection[$this->objref]->site_id;
            $parts = explode("_", $item->id);
            if(!isset($parts[2])) continue;
            $content_type = $parts[1];
            $content_id = $parts[2];
            $needs_protection = false;
            $obj = new indyContentObject($item->id, $content_type);
            $content_obj = $this->getContentItem($content_type, $content_id);
            if($content_obj === false) continue;
            if(!$this->loadSiteObject($obj_set))
            {
               continue;
            }
            $st = $this->site_object->getSiteSettings();
            $ar_id = $st["articleobject"];
            $content = $obj->getHTML($cls_on, BAR_DEPTH, $content_obj, $s, $ar_id, $site_id);
         }
         else
         {
            $obj = $obj_set->fetchObject($item->getId(), $this->getLangCode());
            $obj_stub = $obj_set->itemCollection[$obj->objref];
            $site_id = $obj_stub->site_id;

            if( !$obj or !$obj_stub)
            {
               $this->setUserMessages( $obj_set->getUserMessages() );
               continue;
            }

            if($obj->isProtected() or $item->isORProtected())
            {
               $needs_protection= true;
            }
            $content = $obj->getObjectEditingHTML($obj_set, "", "");
            $content .= $obj->getHTML($cls_on, LONG_MENU_DEPTH, $obj_set, $s, $live);
         }
         if($needs_protection && $live)
         {
            if(!$this->checkObjectAccess($site_id)) continue;
         }

         if((!$live) && $needs_protection)
         {
            $ret .= $this->getProtectionHTMLHead($site_id) . $content. $this->getProtectionHTMLFoot();
         }
         else
         {
            $ret .= $content;
         }
         if($this->getMeta("boxdisplay") == 'horizontal')
         {
            $ret .= "</td>";
         }
      }
      if($this->getMeta("boxdisplay") == 'horizontal')
      {
         $ret .= "</tr></table>";
      }
      $ret .= $this->getFootHTML($cls_str, true);
      return $ret;
   }
}

?>

