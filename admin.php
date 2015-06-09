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



require_once("oscailt_init.inc");

require_once('objects/adminpage.inc');

$redirectList->load();



if(isset($_GET['rescue_mode']))

{

   if($_GET['rescue_mode'] == 'on')

   {

      $_SESSION['rescue_mode'] = true;

   }

   else

   {

      unset($_SESSION['rescue_mode']);

   }

}

addToPageTitle('Site Administration');

$OSCAILT_SCRIPT = 'admin.php';



$admin_obj= new AdminPageObject();

ob_start();

if($editor_session->isSessionOpen())

{

    $admin_obj->processRequest();

}

else

{

   $admin_obj->writeLoginBox(true);

}



require_once('adminfooter.inc');

?>