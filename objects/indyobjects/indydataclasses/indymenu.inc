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


require_once( "indylist.inc" );
/************************************************************************/
/* Class to represent a page component (such as a side menu).
/* Output to a cache file when any of its elements is updated.
/************************************************************************/
class indyMenuObject extends indyListObject
{
   function indyMenuObject ( $a, $b)
   {
      $this->indyListObject ($a, $b);
   }

   var $display_options = array("Menu");



   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      global $system_config;
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      $ret = $this->getHeadHTML($cls_str, false);
      $ret.= $this->getObjectEditingHTML($obj_set, "", "");

      foreach( $this->containedItems['contained'] as $item )
      {
         if($depth < LONG_MENU_DEPTH) $next_depth = LONG_MENU_DEPTH;
         else $next_depth = SHORT_MENU_DEPTH;
         $s = $item->getAsCallerSettings();
         //because menus have a default div wrap -> invert the setting...
         if(!isset($s['surround']) or $s['surround'] != 'on') $s['surround'] = 'on';
         else $s['surround'] = "";

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
            $html = $obj->getHTML($cls_on, BAR_DEPTH, $content_obj, $s, $ar_id, $site_id);
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
            $html = $obj->getObjectEditingHTML($obj_set, "", "");
            $html .= $obj->getHTML($cls_on, $next_depth, $obj_set, $s, $live);
         }

         if($needs_protection && $live)
         {
            if(!$obj->checkObjectAccess($site_id)) continue;
         }
         if((!$live) && $needs_protection)
         {
            $ret .= $obj->getProtectionHTMLHead($site_id) . $html. $obj->getProtectionHTMLFoot();
         }
         else
         {
            $ret .= $html;
         }
      }
      $ret .= $this->getFootHTML($cls_str, false);
      return $ret;
   }
}
?>