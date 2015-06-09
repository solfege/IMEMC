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
   Class Language
   {
      var $language_id;
      var $language_name;
      var $language_code;
      var $active;
      var $excluded;

      function Language()
      {}

      function createFakeLanguage()
      {
         $this->language_id = 0;
         $this->language_name = "No language";
         $this->language_code = "ot";
         $this->active = true;
         $this->excluded = false;
      }

      function getName()
      {
         return $this->language_name;
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->language_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_languages values ('null', '$this->language_name', '$this->language_code',". $this->active.",".$this->excluded.")", $dbconn, 1));
            $this->language_id = mysql_insert_id($dbconn);
         }
         else
         {
            checkForError(sql_query("UPDATE ".$prefix."_languages SET language_name = '$this->language_name', language_code = '$this->language_code', active = '$this->active', excluded = '$this->excluded' WHERE language_id = $this->language_id", $dbconn, 1));
         }
      }

      function load()
      {
         if($this->language_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT language_name, language_code, active, excluded from ".$prefix."_languages where language_id=$this->language_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->language_name, $this->language_code, $this->active, $this->excluded) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Language with ID $this->language_id exists");
            }
         }
         else
         {
            reportError("The language id is null");
         }
      }

      function delete()
      {
         global $languageList;
         if($this->language_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_languages where language_id=$this->language_id", $dbconn,1);
            checkForError($result);
            $languageList->reset();
         }
         else
         {
            reportError("The language id is null");
         }
      }

      function getStoryCount()
      {
         $count=0;
         if($this->language_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT count(*) from ".$prefix."_stories where language_id=$this->language_id", $dbconn, 2);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($count) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Language with ID $this->language_id exists");
            }
         }
         else
         {
            reportError("The language id is null");
         }
         return $count;
      }

      function getLanguageCodePrefix()
      {
         if(strpos($this->language_code, '-') !== false)
         {
            $codes=explode('-',$this->language_code);
         }
         elseif(strpos($this->language_code, '_') !== false)
         {
            $codes=explode('_', $this->language_code);
         }
         else
         {
            return $this->language_code;
         }
         return $codes[0];
      }

      function getLanguageCodeSuffix()
      {
         if(strpos($this->language_code, '-') !== false)
         {
            $codes=explode('-',$this->language_code);
         }
         elseif(strpos($this->language_code, '_') !== false)
         {
            $codes=explode('_', $this->language_code);
         }
         else
         {
            return false;
         }
         return $codes[1];
      }

      function getStrippedName()
      {
         return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->language_name));
      }

      function getStrippedCode()
      {
         return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->language_code));
      }
   }


   Class LanguageList
   {
      var $languages;
      var $excluded;
      var $acceptLanguage;
      var $max_language;
      var $min_language;

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT language_id, language_name, language_code, active, excluded from ".$prefix."_languages", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            $this->languages = array();
            $this->excluded = array();
            $this->max_language = 0;
            $this->min_language = 999;
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
               $l = new Language();
               list($l->language_id, $l->language_name, $l->language_code, $l->active, $l->excluded) = sql_fetch_row($result, $dbconn);
               $this->languages[$l->language_id] = $l;
               if($l->excluded) $this->excluded[] = $l->language_id;
               if($l->language_id > $this->max_language) $this->max_language = $l->language_id;
               if($l->language_id < $this->min_language) $this->min_language = $l->language_id;
            }
         }
         else
         {
            reportError("No Languages present in database");
         }
      }

      function getMinLanguageId()
      {
         if($this->languages==NULL) $this->load();
         return $this->min_language;
      }

      function getMaxLanguageId()
      {
         if($this->languages==NULL) $this->load();
         return $this->max_language;
      }

      function getfLanguages()
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getLanguages();
         return $this->getNonExcludedLanguages();
      }

      function getNonExcludedLanguages()
      {
         if($this->languages==NULL) $this->load();
         $non_excluded = array();
         foreach(array_keys($this->languages) as $l_id)
         {
            if(!in_array($l_id, $this->excluded))
            {
               $non_excluded[] = $this->languages[$l_id];
            }
         }
         return $non_excluded;
      }


      function getAllLanguagesAsPrefixNameOptions($selected_code_prefix="")
      {
         global $editor_session;
         $languages = $this->getfLanguages();
         $str = "";
         for ($i=0; $i<count($languages); $i++)
         {
            if($languages[$i]->active==true || $editor_session->isSessionOpen())
            {
               if($languages[$i]->getLanguageCodePrefix() == $selected_code_prefix)
               {
                  $str .= "<option selected value='".$languages[$i]->getLanguageCodePrefix()."'>".$languages[$i]->language_name."</option>\n";
               }
               else
               {
                  $str .= "<option value=".$languages[$i]->getLanguageCodePrefix().">".$languages[$i]->language_name."</option>\n";
               }
            }
         }
         return $str;
      }

      function getLanguages()
      {
         if($this->languages==NULL) $this->load();
         return array_values($this->languages);
      }

      function getExcludedLanguages()
      {
         if($this->languages==NULL) $this->load();
         $excluded = array();
         foreach($this->excluded as $e)
         {
            $excluded[] = $this->languages[$e];
         }
         return $excluded;
      }

      function getLanguageByName($name)
      {
         if($this->languages==NULL) $this->load();
         foreach($this->languages as $l)
         {
            if (!strcasecmp($l->language_name,$name)) return $l;
         }
         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));
         foreach($this->languages as $l)
         {
            if (!strcasecmp($l->getStrippedName(),$name)) return $l;
         }
         return NULL;
      }

      function getLanguageByPrefix($code)
      {
         foreach($this->languages as $language)
         {
            if(strtolower($language->getLanguageCodePrefix()) == strtolower($code))
            {
               return $language;
            }
         }
         return NULL;
      }

      function getLanguageByCode($code)
      {
         $split=@preg_split("/-/",$code);
         $code_prefix=$split[0];
         if(isset($split[1]))
         {
            $code_suffix=$split[1];
         }
         if($this->languages==NULL) $this->load();
         foreach($this->languages as $l)
         {
            if (!strcasecmp($l->language_code,$code)) return $l;
         }
         // exact match failed so try inexact
         $code=strtolower(str_replace(array("\\","/"," ","-","_"),"",$code));
         foreach($this->languages as $l)
         {
            if (!strcasecmp($l->getStrippedCode(),$code)) return $l;
         }
         // exact match failed on whole code so try first part
         foreach($this->languages as $l)
         {
            if (!strcasecmp($l->getLanguageCodePrefix(),$code_prefix)) return $l;
         }
         return NULL;
      }

      function getLanguageByID($language_id)
      {
         if(is_array($language_id)) $language_id = array_shift($language_id);
         if($this->languages==NULL) $this->load();
         if(isset($this->languages[$language_id])) return $this->languages[$language_id];
         return NULL;
      }

      function reset()
      {
         $this->languages = NULL;
      }

      function undump($dir)
      {

      }

      function dump($dir, $ids = null)
      {
          foreach($this->languages as $l)
          {
             if($ids === null or in_array($l->language_id, $ids))
             {
                $file = $dir."/".$l->language_code.".dump";
                $fp = fopen($file,"w");
                if (flock($fp, LOCK_EX))
                {
                   fputs($fp, serialize($l));
                   flock($fp, LOCK_UN);
                   fclose($fp);
                }
                else
                {
                   reportError("<P>failed to get lock on publish cache file $system_config->publish_monitor_cache_store</p>");
                   return false;
                }
             }
          }
          return true;
      }

      // gets the language from our list of configured languages
      // that is closest match to any preferences specified in the current http request
      // or null if an exact or close match cannnot be found
      function getAcceptLanguage()
      {
         if($this->acceptLanguage!=NULL) return $this->acceptLanguage;
         if($this->languages==NULL) $this->load();
         $alparts=@preg_split("/,/",$_SERVER["HTTP_ACCEPT_LANGUAGE"]);
         /* Parse the contents of the Accept-Language header.*/
         // put into array of code and score
         foreach($alparts as $part)
         {
            $part=trim($part);
            if(preg_match("/;/", $part))
            {
               $splitcode=@preg_split("/;/",$part);
               $splitscore=@preg_split("/=/",$splitcode[1]);
               $alscores[strtolower($splitcode[0])]=$splitscore[1];
            }
            else
            {
               $alscores[$part]=1;
            }
         }
         // sort by score
         arsort($alscores,SORT_NUMERIC);
         // go through each preference and try find match or near match
         // if none found continue to next preferenence and so on
         foreach(array_keys($alscores) as $al)
         {
            //echo ("<BR>score:".$al." ".$alscores[$al]."<BR>");
            $acceptLanguage = new Language();
            $acceptLanguage->language_code=$al;
            // for each language configured perform check
            // allow subsequent discovery of exact match to overwrite near match
            // prevent subsequent discovery of near match from overwriting exact match
            foreach ($this->languages as $language)
            {
               if(strtolower($language->language_code)==$acceptLanguage->language_code)
               {
                  $this->acceptLanguage = $language;
               }
               else if($this->acceptLanguage==null && strtolower($language->getLanguageCodePrefix())==$acceptLanguage->getLanguageCodePrefix())
               {
                  $this->acceptLanguage = $language;
               }
            }
            // if match found return otherwise continue search with next preference
            if($this->acceptLanguage!=null) return $this->acceptLanguage;
         }
         // no match found
         return null;
      }

      function getLanguagesSelect($FilteredSelection)
      {
         if($this->languages==NULL) $this->load();
        
         $js_string = "";

         /*
	 if ($FilteredSelection == 0 )
             $form_string ="<option selected value="0">All Languages</option>";
	 else
             $form_string ="<option value="0">All Languages</option>";
         */
         $form_string ="";

         $form_string .="<option value=\"";
         foreach ( $this->languages as $each_lang)
         {
             if ($each_lang->language_id == $FilteredSelection)
             {
                  $js_string .= "<option selected value=\"" . $each_lang->language_id ."\">" . ucfirst($each_lang->getName()) . "</option>";
             }
             else
             {
                  $js_string .= $form_string . $each_lang->language_id ."\">" .ucfirst($each_lang->getName())."</option>";
             }
         }  
    
         return $js_string;
      }
   }
?>
