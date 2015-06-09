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
/* Class to represent exported feed data
/************************************************************************/
class indyFeedexportObject extends indySiteModuleObject
{
   var $display_options = array(
      "None"
   );

   var $runtime_requires = array("feedexportdisplay.inc");

    function indyFeedexportObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }

    function getDefaultDisplayMode($depth)
    {
       return "None";
   }

    function getBasicFilterPrefix()
   {
      return "feedexportfilter";
   }




   function getFeedXML(&$obj_set, $live)
   {
      if(!$this->loadSiteObject($obj_set))
      {
         $ret .= "INTERNAL ERROR";
         return $ret;
      }
      $param_str = $this->getAllParams("feedexportfilter");
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('feedExportDisplay', $param_str, $siteset_str);
      $end_str = $this->getPHPEndString();

      //necessary as this object is not wrapped in a page
      //this gives us a php string wich sets the basic information about the object on the page
      //and identifies the site structure that it belongs to
      //for, example, this allows us to get the appropriate contact email address for the page.
      $page_str = $this->getPageSettingString($obj_set);

      //any configuration information that you want to add to the module needs to go here
      //as well as into the fields of the xml file
      //just put the names of the fields from the xml files into these arrays and they will
      //be reproduced in the object cache file produced.
      //the first array gets any selects / text / textarea / radio fields
      //the second array gets any checkboxes.
      $basic_text_fields = array("feedlogo", "feedicon", "feedtitle", "feeddescription","feedlicence", "feedlength", "feedsummarylength", "filetypes","feeddepth","feedformat","itunestags","itunescategory","itunessubcategory","itunesauthor","itunesownername","itunesowneremail","itunessummary","itunessubtitle","ituneskeywords","itunesimage","itunesexplicit", "itunesblock");
      $basic_checkbox_fields = array("filecentric","parsecontent");
      $admin_settings = array();
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $display_param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$display_param_str.');'."\n";

      //then we repeat the same above with any variables that we want the user to be able
      //to set from the query.  Essentially, whatever is set in the configuration page for this type
      //of setting becomes the _default_ option for the feed.
      //if we do it like this, we get the user submitted variables delivered
      //to our runtime object already merged with the admin defined variables
      //_and_ already security screened -> less for us to worry about and neater all around
      //in this case I'm going to allow the user to specify the number of items returned in the
      //query string and whether the request is file centric as examples.
      $basic_text_fields = array("feedtype", "feedversion");
      $basic_checkbox_fields = array();
      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);
      $display_param_str = serialiseArrayParams($user_settings);
      $user_str = '$site_module->setUserOptions('.$display_param_str.');'."\n";
        $php_str = '<?php '.$page_str."\n".$base_str.$admin_str.$user_str.$end_str.' ?>';
        $ret = $php_str;
        return $ret;
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
    {
      if(!$this->loadSiteObject($obj_set))
      {
         $ret .= "INTERNAL ERROR";
         return $ret;
      }
      return $this->getFeedXML($obj_set, $live);
    }




}
?>