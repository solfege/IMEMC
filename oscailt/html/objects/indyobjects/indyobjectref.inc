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

/************************************************************************/
/* A class that represents a reference to a contained indy object.
/************************************************************************/
class indyObjectRef extends indyObject
{
    var $id;
    var $override_protected;
   var $settings = array();

    function indyObjectRef ($i, $settings)
    {
        $this->id = $i;
        if(isset($settings['override_protected']) && $settings['override_protected'] == 'on')
        {
           $this->override_protected = $settings['override_protected'];
        }
        $this->settings = $settings;
    }

   function isORProtected()
   {
      return $this->override_protected;
   }

   function getAsCallerSettings()
   {
      return $this->settings;
   }

    function getId ()
    {
        return $this->id;
    }

    function Id ()
   {
       return $this->id;
    }

    function getStyle ()
    {
        return $this->getSetting('style');
    }

    function getDepth ()
    {
        return $this->getSetting('depth');
    }

   function getOverrideProtected ()
   {
      return $this->getSetting('override_protected');
   }

   function setSetting($n, $v)
   {
      $this->settings[$n] = $v;
   }

   function getSetting($n)
   {
      if(isset($this->settings[$n])) return $this->settings[$n];
      else return false;
   }

   function getXML()
   {
      $str = "";

      foreach(array_keys($this->settings) as $setting)
      {
         $str .= " ".$setting ."='".$this->settings[$setting]."'";
      }
      $str = "<indyObjectRef idref='$this->id'".$str." />";
      return $str;
   }

    function getBox ()
    {
        return $this->getSetting('surround');
    }

}
?>