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

// This file defines some keywords which can be translated. They are too general to
// be specific to any object or module.
//
// Note: Spanish is normally coded as es, but it is also know as Castellano with the code ca.
//       So the match is to test es or ca.

class languageKeywords
{
   var $lingo;
   var $known_languages;
   var $known_keywords;

   function languageKeywords()
   {
      global $userLanguage, $system_config;
      $this->lingo = "en";
      if ($userLanguage != null) {
          $this->lingo = $userLanguage->getLanguageCodePrefix();
      } else {
          // If the default language is not English then override the default to it here.
          if ($system_config->default_language_code != 'en')
              $this->lingo = $system_config->default_language_code; 
      }
      $this->known_languages = array("en", "ca", "es", "fr", "de");
      $this->known_keywords = array("by", "for", "and", "story", "comment");
   }

   function getText($keyword) 
   {
       if (!in_array($this->lingo, $this->known_languages)) return $keyword;

       $keyword == strtolower($keyword);

       // If the word is not in our arrays just return it.
       if (!in_array($keyword, $this->known_keywords)) return $keyword;

       if ($this->lingo == "fr") {
           $myArray = Array( "by" => "par", "for" => "pour", "and" => "et", "story" => "story", "comment" => "comment");

       } else if ($this->lingo == "es" || $this->lingo == "ca") {
           $myArray = Array( "by" => "por", "for" => "para", "and" => "y", "story" => "story", "comment" => "comment");

       } else if ($this->lingo == "de") {
           $myArray = Array( "by" => "bei", "for" => "fur", "and" => "und", "story" => "story", "comment" => "comment");

       } else if ($this->lingo == "sv") {
           $myArray = Array( "by" => "hos", "for" => "för", "and" => "och", "story" => "historia", "comment" => "anmärkning");

       } else {
           $myArray = Array( "by" => "by", "for" => "for", "and" => "and", "story" => "story", "comment" => "comment");
       }
       return ($myArray[$keyword]." ");
    }
    
    function isEnglish() 
    {
       if ($this->lingo == "en") return true;

       return false;
    }
}
?>
