<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "84", "obj_type" => "CONTACT", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Formulario De Contacto");
         addToPageDescription("El formulario de contacto para ponerse en contacto con los editores"); ?><?php require_once "objects/indyruntime/contactdisplay.inc";

         $site_module = new contactDisplay(84);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Contacte con nosotros", "header" => "", "footer" => "", "nametitle" => "Nombre", "contentstitle" => "Mensaje", "subjecttitle" => "Asunto", "fromtitle" => "Correo ElectrÃ?Â³nico", "introtext" => "Si desea ponerse en contacto o participar en el colectivo Indymedia, por favor, rellene el Formulario de correo electrÃ?Â³nico a continuaciÃ?Â³n y haga clic en \'Enviar mensaje\'.
<br />
Usted debe completar en todos los Ã?Â¡mbitos que presente un mensaje de correo electrÃ?Â³nico.", "emailto" => "news@imemc.org", "emailfrom" => "IMEMC Contact Form", "emailreply" => "", "formheadtext" => "Rellene el siguiente formulario para presentar un mensaje a los editores.", "buttontext" => "Enviar", "successtext" => "Su mensaje ha sido enviado a los administradores que manejan este sitio. Si incluyÃ?Â³ una direcciÃ?Â³n vÃ?Â¡lida de correo electrÃ?Â³nico, podemos ponernos en contacto con usted en breve una respuesta. No podemos garantizar una respuesta dentro de cualquier marco temporal. Haremos todo lo posible, sin embargo, recibirÃ?Â¡ una respuesta con seguridad.", "finalchecktext" => "", "spammandatorywords" => "", "editorprivatemail" => "false", "logemails" => "false", "hidesubjectfield" => "false", "spambotcheck" => "false", "logspamcheck" => "false"));

$site_module->setUserOptions(array());

         $site_module->processRequest();
       ?><?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "84"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcontact", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=contact&amp;target_id=84&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Contact Form Module [Formulario de contacto] Configuration" alt="Edit Contact Form Module [Formulario de contacto] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial.png" /></a>
             
          <?
          }
          ?>
          