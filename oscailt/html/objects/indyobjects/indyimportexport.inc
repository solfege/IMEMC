<?
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
require_once "objects/indyobjects/indydatacollection.inc";

class indyImportExport extends indyObject
{
   var $root_dir;
   var $choices = array();
   var $obj_set;
   var $data_collection;
   var $metadata;
   var $user_error;

   function indyImportExport($root, &$obj_set)
   {
      $this->root_dir = $root;
      $this->obj_set = $obj_set;
      $this->clearChoices();
   }

   function clearChoices()
   {
       $this->choices = array(
            "language" => array(),
            "region" => array(),
            "topic" => array(),
            "type" => array(),
            "role" => array(),
            "new_language" => array(),
            "new_region" => array(),
            "new_topic" => array(),
            "new_type" => array(),
            "new_role" => array(),
            "obj" => array(),
            "stories" => array(),
            "target" => array(),
            "css" => null,
            "links" => null,
            "files" => null,
            "redirects" => null,
            "update urls" => null);
//       $this->data_collection = null;
   }

   function loadCollection($target)
   {
      $this->data_collection = new indyDataCollection($this->root_dir, $this->root_dir.$target."/", $target);
      $this->data_collection->load();
   }

   function readChoicesFromCollection($c)
   {
      $this->loadCollection($c);
      $this->metadata =& $this->data_collection->info;
      $this->data_collection->compareToInstallation($this->choices, $this->obj_set);
   }

   function getUserMessage()
   {
      return $this->user_error;
   }

   function & getCollectionListInfo()
   {
      $dh=opendir($this->root_dir);
      if(!$dh)
      {
         reportError("Couldn't open export directory: ".$this->root_dir);
         return false;
      }
      $cols = array();
      while($file=readdir($dh))
      {
         if(is_dir($this->root_dir."/".$file))
         {
            if($file=="." || $file=="..") continue;
            if(file_exists($this->root_dir.$file.".xml"))
            {
               //$cols[] = $file;
               $c = new indyItemSet();
               $c->load($this->root_dir, $file);
               $cols[] = $c;
            }
         }
         else
         {
            continue;
         }
      }
      closedir($dh);
      return $cols;
   }

   function writeChooseCollectionSection($cols, &$s_o, $form_type = "export", $col_id = null)
   {
      global $system_config, $OSCAILT_SCRIPT;
      $info_items = array("language", "region", "type", "topic", "role");
      $collections =& $this->getCollectionListInfo();
      foreach($collections as $c)
      {
         if($col_id !== null and $col_id !== $c->name) continue;
         writeMenuSpacer();
         writeMenuHeader();
         $c_name = $c->name;
         $c_title = $c->getMeta("title");
         $c_description = $c->getMeta("description");
         $c_icon = $c->getMeta("iconsmall");
         if($c_icon != "" && file_exists($c_icon))
         {
            $c_intro = "<img src='$c_icon'> $c_description";
         }
         else
         {
            $c_intro = $c_description;
         }
         echo "<tr class='admin'><th class='admin' colspan='$cols'><big>$c_title</big> ($c_name collection) </th></tr>";
         if($form_type == "export")
         {
            $view_txt = $s_o->getString("view_export_text");
            $filling = "<a href='?action=view&amp;target_collection=$c->name'>$view_txt <img src='graphics/view.gif' alt='view'></a>";
         }
         elseif($form_type == 'install' && $col_id == null)
         {
            $view_txt = $s_o->getString("view_export_text");
            $filling ="<a href='?file_stage_done=confirmed&amp;database_stage_done=confirmed&amp;&amp;dataobjects_choices_done=no&amp;collection_choice=$c->name'>$view_txt <img border = 0 src='graphics/view.gif' alt='view'></a>";
         }
         else
         {
            $filling = "";
         }
         echo "<tr class='admin'><td class='admin' colspan='$cols'>$c_intro $filling </th></tr>";
	 if (isset($_REQUEST['summary']) && ($_REQUEST['summary'] == 'true' || $_REQUEST['summary'] == '1')) {
	     $show_summary = true;
	 } else {
	     $show_summary = false;
	 }

	 if ($show_summary != true) {
             if($form_type != 'install' or $col_id != null)
             {
                $offset = 0;
                foreach($info_items as $i)
                {
                   $item = $c->getItemByName($i);
                   if(!$item) continue;
                   if($offset % $cols == 0)
                   {
                       echo "<tr class='admin'>";
                   }
                   $offset++;
                   echo "<td class='admin'>";
                   echo "<h4 class='object-name-list'>".$item->getMeta("title")."</h4>";
                   echo "<ul class='object-name-list'>";
                   $e = $item->getMeta("entries");
                   if($e != "")
                   {
                      $entries = explode(",", $e);
                      foreach($entries as $e)
                      {
                         echo "<li class='object-name-list'>$e</li>";
                      }
                   }
                   else
                   {
                      echo "<li class='object-name-list'><i>No Entries</i></li>";
                   }
                   echo "</ul>";
                   echo "</td>";
                   if($offset % $cols == 0)
                   {
                     echo "</tr>";
                   }
                }
                $item = $c->getItemByName("content");
                if($item)
                {
                   if($offset % $cols == 0)
                   {
                      echo "<tr class='admin'>";
                   }
                   $offset++;
                   echo "<td class='admin'>";
                   echo "<h4 class='object-name-list'>".$item->getMeta("title")."</h4>";
                   echo "<ul class='object-name-list'>";
                   foreach(array("stories", "comments", "attachments") as $con)
                   {
                      $num = $item->getMeta($con);
                      echo "<li class='object-name-list'>$con ($num)</li>";
                   }
                   if($offset % $cols == 0)
                   {
                      echo "</tr>";
                   }
                }
                $item = $c->getItemByName("sundries");
                if($item)
                {
                   if($offset % $cols == 0)
                   {
                      echo "<tr class='admin'>";
                   }
                   $offset++;
                   echo "<td class='admin'>";
                   echo "<h4 class='object-name-list'>".$item->getMeta("title")."</h4>";
                   echo "<ul class='object-name-list'>";
                   foreach(array("css", "files", "redirects") as $con)
                   {
                      $num = $item->getMeta($con);
                      echo "<li class='object-name-list'>$con ($num)</li>";
                   }
                   if($offset % $cols == 0)
                   {
                      echo "</tr>";
                   }
                }
                $items = $c->getItemsByType("objectlist");
                foreach($items as $item)
                {
                   if($offset % $cols == 0)
                   {
                      echo "<tr class='admin'>";
                   }
                   $offset++;
                   echo "<td class='admin'>";
                   echo "<h4 class='object-name-list'>".$item->getMeta("title")." Section</h4>";
                   echo "<ul class='object-name-list'>";
                   foreach(array("objects" => "Data Objects", "attached_files" => "Included Files") as $k => $v)
                   {
                      $num = $item->getMeta($k);
                      echo "<li class='object-name-list'>$v ($num)</li>";
                   }
                   if($offset % $cols == 0)
                   {
                      echo "</tr>";
                   }
                }
                if($offset % $cols != 0)
                {
                   while($offset % $cols != 0)
                   {
                      echo "<td class='admin'>&nbsp;</td>";
                      $offset++;
                   }
                   echo "</tr>";
                }
             }
         }
         if($form_type == 'import' && $col_id === null)
         {
            $this->writeImportButtons($cols, $s_o, $c_name);
         }
         elseif($form_type == 'install')
         {
            $this->writeInstallButtons($cols, $s_o, $c_name, $col_id!=null);
         }
         writeMenuFooter($cols, false);
      }
   }

   function getChoicesInfo()
   {
      $str = "<div style='text-align: left; margin: 1em;'>";
      $str.= "<P><BIG>Selected for Export to ";
      $str .= $this->metadata->name . " Collection";
      $str .= "</BIG></P>";
      foreach($this->choices as $k => $v)
      {
         if($k == "target") continue;
         else $str .= "<b>$k</b> [";
         if($k == "stories")
         {
            if(isset($this->choices['stories'][0]))
            {
               $str .= "(".$this->getStoryCount().") select where ".$this->choices['stories'][0];
               if(isset($this->choices['stories'][1]) && $this->choices['stories'][1])
               {
                  $str .= " | with comments";
               }
               else
               {
                  $str .= " | no comments";
               }
               if(isset($this->choices['stories'][2]) && $this->choices['stories'][2])
               {
                  $str .= " | with attachments";
               }
               else
               {
                  $str .= " | no attachments";
               }
            }
            else
            {
               $str .= "no";
            }
         }
         elseif($k == "css" or $k == "files" or $k == "redirects" or $k == "links" or $k == "update urls")
         {
            if(isset($this->choices[$k]) && $this->choices[$k] === true) $str .= "yes";
            else $str .= "no";
         }
         else
         {
            $str .= implode(" | ", $v)."";
         }
         $str .= "]<br>";
      }
      $str .= "</div>";
      return $str;
   }

}
?>
