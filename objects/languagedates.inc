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


// This class defines various things like the names of the days of the
// week and the months but it is language specific. Then when the object
// is created it can be used to retrive the values.
// There is probably no need to make this a class. It could just be a series of functions.
//
// Note: Spanish is normally coded as es, but it is also know as Castellano with the code ca.
//       So the match is to test es or ca.

class languageDates
{
   var $lingo;

   function languageDates()
   {
      global $userLanguage;
      $this->lingo = "en";
      if ($userLanguage != null) {
          $this->lingo = $userLanguage->getLanguageCodePrefix();
      }
   }

   function weekDayLongString($weekday) 
   {
       if ($this->lingo == "fr") {
           $myArray = Array( 0 => "Dimanche", 1 => "Lundi", 2 => "Mardi", 3 => "Mercredi",
                             4 => "Jeudi", 5 => "Vendredi", 6 => "Samedi");
       } else if ($this->lingo == "es" || $this->lingo == "ca") {
           $myArray = Array( 0 => "Domingo", 1 => "Lunes", 2 => "Martes", 3 => "Miércoles",
                             4 => "Jueves", 5 => "Viernes", 6 => "Sábado");
       } else if ($this->lingo == "pt") {
           $myArray = Array( 0 => "Domingo", 1 => "Segunda", 2 => "Terça", 3 => "Quarta",
                             4 => "Quinta", 5 => "Sexta", 6 => "Sábado");
       } else if ($this->lingo == "de") {
           $myArray = Array( 0 => "Sonntag", 1 => "Montag", 2 => "Dienstag", 3 => "Mittwoch",
                             4 => "Donnerstag", 5 => "Freitag", 6 => "Samstag");
       } else if ($this->lingo == "it") {
           $myArray = Array( 0 => "Domenica", 1 => "Lunedì", 2 => "Martedì", 3 => "Mercoledì",
                             4 => "Giovedì", 5 => "Venerdì", 6 => "Sabato");
       } else {
           $myArray = Array( 0 => "Sunday", 1 => "Monday", 2 => "Tuesday", 3 => "Wednesday",
                             4 => "Thursday", 5 => "Friday", 6 => "Saturday");
       }
       return $myArray[$weekday];
    }
    
    // This returns a 2 character abbrevation of the name of the day.
    // This function appears to be not used.
    function weekDayString($weekday) 
    {
       if ($this->lingo == "fr") {
           $myArray = Array( 0 => "Di", 1 => "Lu", 2 => "Ma", 3 => "Me", 4 => "Je", 5 => "Ve", 6 => "Sa");

       } else if ($this->lingo == "es" || $this->lingo == "ca") {
           $myArray = Array( 0 => "Do", 1 => "Lu", 2 => "Ma", 3 => "Mi", 4 => "Ju", 5 => "Vi", 6 => "Sá");
    
       } else if ($this->lingo == "pt") {
           // Portuguese
           $myArray = Array( 0 => "Do", 1 => "Se", 2 => "Tr", 3 => "Qa", 4 => "Qi", 5 => "Sx", 6 => "Sa");
    
       } else if ($this->lingo == "de") {
           $myArray = Array( 0 => "So", 1 => "Mo", 2 => "Di", 3 => "Mi", 4 => "Do", 5 => "Fr", 6 => "Sa");
    
       } else if ($this->lingo == "it") {
           $myArray = Array( 0 => "Do", 1 => "Lu", 2 => "Ma", 3 => "Mr", 4 => "Gi", 5 => "Ve", 6 => "Sa");
    
       } else {
           $myArray = Array( 0 => "Su", 1 => "Mo", 2 => "Tu", 3 => "We", 4 => "Th", 5 => "Fr", 6 => "Sa");
       }
       return $myArray[$weekday];
    }
    
    // This returns a 2 character abbrevation of the name of the day already formated in a table TH tag.
    function getWeekDaysBar() 
    {
       if ($this->lingo == "fr") {
           $weekDayTableBar = "<th>Di</th> <th>Lu</th> <th>Ma</th> <th>Me</th> <th>Je</th> <th>Ve</th> <th>Sa</th>";

       } else if ($this->lingo == "es" || $this->lingo == "ca") {
           $weekDayTableBar = "<th>Do</th> <th>Lu</th> <th>Ma</th> <th>Mi</th> <th>Ju</th> <th>Vi</th> <th>Sá</th>";
    
       } else if ($this->lingo == "pt") {
           $weekDayTableBar = "<th>Do</th> <th>Seg</th> <th>Ter</th> <th>Qua</th> <th>Qui</th> <th>Sex</th> <th>Sa</th>";
       } else if ($this->lingo == "de") {
           $weekDayTableBar = "<th>So</th> <th>Mo</th> <th>Di</th> <th>Mi</th> <th>Do</th> <th>Fr</th> <th>Sa</th>";
    
       } else if ($this->lingo == "it") {
           $weekDayTableBar = "<th>Do</th> <th>Lu</th> <th>Ma</th> <th>Mr</th> <th>Gi</th> <th>Ve</th> <th>Sa</th>";
    
       } else {
           $weekDayTableBar = "<th>Su</th> <th>Mo</th> <th>Tu</th> <th>We</th> <th>Th</th> <th>Fr</th> <th>Sa</th>";
       }
       return $weekDayTableBar;
    }
    
    function isEnglish() 
    {
       if ($this->lingo == "en") return true;

       return false;
    }

    function monthString($month_number) 
    {
       if ($this->lingo == "fr") {
           $myArray = Array( 0 => "Janvier", 1 => "Février", 2 => "Mars", 3 => "Avril", 4 => "Mai", 5 => "Juin", 6 => "Juillet", 7 => "Août", 8 => "Septembre", 9 => "Octobre", 10 => "Novembre", 11 => "Décembre");
    
       } else if ($this->lingo == "es" || $this->lingo == "ca") {
           $myArray = Array( 0 => "Enero", 1 => "Febrero", 2 => "Marzo", 3 => "Abril", 4 => "Mayo", 5 => "Junio", 6 => "Julio", 7 => "Agosto", 8 => "Septiembre", 9 => "Octubre", 10 => "Noviembre", 11 => "Diciembre");
    
       } else if ($this->lingo == "pt") {
           // Portuguese
           $myArray = Array( 0 => "Janeiro", 1 => "Fevereiro", 2 => "Março", 3 => "Abril", 4 => "Maio", 5 => "Junho", 6 => "Julho", 7 => "Agosto", 8 => "Setembro", 9 => "Outubro", 10 => "Novembro", 11 => "Dezembro");
    
       } else if ($this->lingo == "de") {
           $myArray = Array( 0 => "Januar", 1 => "Februar", 2 => "März", 3 => "April", 4 => "Mai", 5 => "Juni", 6 => "Juli", 7 => "August", 8 => "September", 9 => "Oktober", 10 => "November", 11 => "Dezember");
    
       } else if ($this->lingo == "it") {
           $myArray = Array( 0 => "Gennaio", 1 => "Febbraio", 2 => "Marzo", 3 => "Aprile", 4 => "Maggio", 5 => "Giugno", 6 => "Luglio", 7 => "Agosto", 8 => "Settembre", 9 => "Ottobre", 10 => "Novembre", 11 => "Dicembre");
    
       } else {
           $myArray = Array( 0 => "January", 1 => "February", 2 => "March", 3 => "April", 4 => "May", 5 => "June", 6 => "July", 7 => "August", 8 => "September", 9 => "October", 10 => "November", 11 => "December");
       }
    
       return $myArray[$month_number-1];
    }

    // This function handles language version for things like: 1st, 2nd, 3rd, 4th, 5th
    function getNumber_Th_Text($day_value )
    {
       if ($this->lingo == "es" || $this->lingo == "ca") {
           $th_text = "*";
           if ($day_value > 3 && $day_value < 21 ) return $th_text;
           else if ($day_value > 23 && $day_value < 31 ) return $th_text;
           else if ($day_value == 1 ) $th_text = "ero";
           else if ($day_value == 2 ) $th_text = "do";
           else if ($day_value == 3 ) $th_text = "er";
           else if ($day_value == 21 ) $th_text = "vo";
           else if ($day_value == 22 ) $th_text = "vo";
           else if ($day_value == 23 ) $th_text = "vo";
           else if ($day_value == 31 ) $th_text = "vo";
       } else {
           $th_text = "th";
           if ($day_value > 3 && $day_value < 21 ) return $th_text;
           else if ($day_value > 23 && $day_value < 31 ) return $th_text;
           else if ($day_value == 1 ) $th_text = "st";
           else if ($day_value == 2 ) $th_text = "nd";
           else if ($day_value == 3 ) $th_text = "rd";
           else if ($day_value == 21 ) $th_text = "st";
           else if ($day_value == 22 ) $th_text = "nd";
           else if ($day_value == 23 ) $th_text = "rd";
           else if ($day_value == 31 ) $th_text = "st";
       }
       return $th_text;
    }
}
?>
