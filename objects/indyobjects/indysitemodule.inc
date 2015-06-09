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
/* Class to represent a site on the installation                        */
/* may be filtered by any topic, type, language, region, also author    */
/* This is the abstract class that all site-filtered modules inherit    */
/* it provides various common functionality useful to modules.          */
/************************************************************************/
class indySiteModuleObject extends indyBasicObject
{
   var $amalgamated_filters;
   var $runtime_requires = array();

    function indySiteModuleObject ($a, $b)
    {
        $this->indyBasicObject($a, $b);
    }

   /************************************************************************/
   /* Write out the php for including the necessary php files for runtime objects  */
   /************************************************************************/
   function getRequires()
   {
      global $path_prefix;
      $req_php_str = "";
      if(count($this->runtime_requires) > 0)
      {
         foreach($this->runtime_requires as $requ)
         {
            $req_file = $path_prefix.'objects/indyruntime/'.$requ;
            $req_php_str .= 'require_once "'.$req_file.'";'."\n";
         }
      }
      return $req_php_str;
   }

   /************************************************************************/
   /* Write out the php stubs for runtime objects  */
   /************************************************************************/
   function getPHPBaseString($display_type, $filter_str, $site_setting_str)
   {
      $req_php_str = $this->getRequires();
      $basic_php_tmpl_str = $req_php_str .
      '
         $site_module = new [@[@0]@]('.$this->id().');
         $site_module->setFilters[@[@1]@];
         $object_display_settings = [@[@2]@];
         $site_module->setSiteSettings($object_display_settings);
      ';
      return $this->populatePHPTemplateString($basic_php_tmpl_str, array($display_type, $filter_str, $site_setting_str));
   }

   function getPHPEndString()
   {
      $end_php_tmpl_str =
      '
         $site_module->processRequest();
      ';

      return $end_php_tmpl_str;
   }

   function getPHPHeadlineEndString()
   {
      $end_php_tmpl_str =
      '
         $site_module->retrieveHeadlines();
      ';

      return $end_php_tmpl_str;
   }


   /************************************************************************/
   /* Compute the links to modules within the site
   /************************************************************************/
   function getAmalgamatedSiteSettingString()
    {
      $allSettings = $this->site_object->getSiteSettings();
      foreach(array_keys($allSettings) as $key)
      {
         if($this->getMeta($key) != "")
         {
            $allSettings[$key] = $this->getMeta($key);
         }
      }
      return $this->getSiteSettingString($allSettings);

    }

   function getSiteSettingString(&$allSettings)
   {
      //get a string which is the input parameter list to the nw object...
      $str = "";
      $first = true;
      foreach(array_keys($allSettings) as $key)
      {
         if(!$first) $str .= ", ";
         else $first = false;

         if(!isset($allSettings[$key]) or $allSettings[$key] == "")   $str .= '"'.$key.'"'.'=> "0"';
         else  $str .= '"'.$key.'" => "'.$allSettings[$key].'"';
      }
      $str = '(Array('.$str.'))';
      return $str;
   }

   function arrayToString ($arr)
   {
      if(is_array($arr) && (! in_array("None", $arr)))
      {
         $params = 'Array("'. implode('", "', $arr).'")';
      }
      elseif(is_array($arr) && in_array("None", $arr)) $params = 'Array("*")';
      else $params = 'Array()';
      return $params;
   }

   function getBasicFilterPrefix()
   {
      return "";
   }

   function getPageSettingArray(&$obj_set)
   {
      $page_settings = parent::getPageSettingArray($obj_set);
      $page_settings['page_filters'] = $this->getPageFilterArray($obj_set);
      return $page_settings;
   }

   function getPageFilterArray(&$obj_set)
   {
      $prefix = $this->getBasicFilterPrefix();
      $this->getAmalgamatedFilters($prefix);
      $arr = $this->amalgamated_filters;
      if(in_array("None", $arr["topic"])) $arr['topic'] = array("*");
      if(in_array("None", $arr["region"])) $arr['region'] = array("*");
      if(in_array("None", $arr["language"])) $arr['language'] = array("*");
      if(in_array("None", $arr["type"])) $arr['type'] = array("*");
      return $arr;
   }

   function getMyFilters($prefix)
   {
      $my_filters = array();
      $my_filters['topic'] = $this->getMeta($prefix.'topic');
      $my_filters['region'] = $this->getMeta($prefix.'region');
      $my_filters['language'] = $this->getMeta($prefix.'language');
      $my_filters['type'] = $this->getMeta($prefix.'type');
      return $my_filters;
   }

   function getAllParams ($prefix="")
   {
      $this->getAmalgamatedFilters($prefix);
      //print_r($this->amalgamated_filters);
      $t = $this->arrayToString($this->amalgamated_filters['topic']);
      $r = $this->arrayToString($this->amalgamated_filters['region']);
      $l = $this->arrayToString($this->amalgamated_filters['language']);
      $ty = $this->arrayToString($this->amalgamated_filters['type']);
      $au = $this->arrayToString($this->amalgamated_filters['author']);
      return '('.$t.', '.$r.', '.$l.', '.$ty.', '.$au.')';
   }

   function getAmalgamatedFilters ($prefix)
   {
      if(!$this->site_object)
      {
         $this->setUserMessage(array(INDY_ERROR,"Site object not loaded for object id ". $this->id(),"238"));
         return false;
      }
      $site_filters = $this->site_object->getSiteFilters("sitebasicfilters");
      if($prefix == "")
      {
         $my_filters = array();
      }
      else
      {
         $my_filters = $this->getMyFilters($prefix);
      }
      $this->combine($site_filters, $my_filters);
      if(trim($this->getMeta("filter_author")) == "" or trim($this->site_object->getMeta("filter_author")) != "")
      {
         $this->amalgamated_filters['author'] = array($this->site_object->getMeta("filter_author"));
      }
      elseif(trim($this->getMeta("filter_author")) != "")
      {
         $this->amalgamated_filters['author'] = array($this->getMeta("filter_author"));
      }
      else
      {
         $this->amalgamated_filters['author'] = "";
      }
      return $this->amalgamated_filters;
   }

   function combine($base, $filter)
   {
      foreach(array_keys($base) as $one_field)
      {
         if(!isset($filter[$one_field]) or $filter[$one_field] == "")
         {
            $filter[$one_field] = array("None");
         }
         if(!is_array($filter[$one_field]))
         {
            $filter[$one_field] = array($filter[$one_field]);
         }

         if(!is_array($base[$one_field]))
         {
            $base[$one_field] = array("None");
         }

         //first chance is that one or t'other contains none...
         if(in_array("None", $base[$one_field])) $this->amalgamated_filters[$one_field] = $filter[$one_field];
         elseif(in_array("None", $filter[$one_field])) $this->amalgamated_filters[$one_field] = $base[$one_field];
         else
         {
            foreach($base[$one_field] as $one_entry)
            {
               if(in_array($one_entry, $filter[$one_field])) $this->amalgamated_filters[$one_field][] = $one_entry;
            }
         }
      }
   }

   function populateBasicSettingFields(&$admin_settings, $text_fields, $checkbox_fields, $extra = "")
   {
      foreach($text_fields as $f)
      {
         $admin_settings[$f] = $this->getMeta($extra.$f);
      }
      foreach($checkbox_fields as $c)
      {
         if($this->getMeta($extra.$c) == 'on')
         {
            $admin_settings[$c] = 'true';
         }
         else
         {
            $admin_settings[$c] = 'false';
         }
      }
      return $admin_settings;
   }

   function populateMultiCheckboxFields(&$allowed_multis, &$optional_multis, $multi)
   {
      foreach($optional_multis as $opt)
      {
         $f = $multi.'_'.$opt;
         if($this->getMeta($f) == 'on')
         {
            $allowed_multis[] = $opt;
         }
      }
      return $allowed_multis;
   }


}
?>