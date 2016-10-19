<?php
// $Id$
//@todo: invalid_option should be label in case of timeout? http://void.media.mit.edu/domestic_workers/?q=admin/reports/event/95439
/**
 * @file
 * Audio Blog VoIP Scripts and helper functions
 */

/**
 * Implementation of hook_voipscript_get_script_names()
 */
function claro_voipscript_get_script_names() {
  $script_names[] = 'claro_welcome_script';
  $script_names[] = 'claro_sms_handler_script';
  return $script_names;
}

/**
 * Implementation of hook_voipscript_load_script()
 */
function claro_voipscript_load_script($script_name, $options = NULL) {

  $script = NULL;
  switch ($script_name) {
    default:
      break;
    case 'claro_welcome_script':
      $script = new VoipScript('claro_welcome_script');
      $script->addSetVoice('woman');
      $options['voice'] = 'claro';
      voipvoice_set_current_voice('claro');
      $script->addSet('max_session_duration', 0);

      // log the call
      $log_msg = t("Call from %caller_number on @date (id: %call_id)",
        array('@date' => format_date(time(), 'MDT')));
      $script->addLog($log_msg, 'claro');


      $script->addLabel('language_selection');
      $p_main_menu[] = v("Para Espanol, oprima uno. For English, press two. Para português, pressione 3. ");
      $input_options = array(
        '1' => 'spanish_version',
        '2' => 'english_version',
        '3' => 'portugese_version',
        'i' => 'hang_up',
        't' => 'hang_up'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_main_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('spanish_version');
      $options['voice'] = 'claro-spanish';
      $script->addGosub('claro_intro_script', $options);
      $script->addGoto('hang_up');

      $script->addLabel('portugese_version');
      $options['voice'] = 'claro-portugese';
      $script->addGosub('claro_intro_script', $options);
      $script->addGoto('language_selection');


      $script->addLabel('english_version');
      $options['voice'] = 'claro';
      $script->addGosub('claro_intro_script', $options);
      $script->addGoto('hang_up');

      //$script->addLabel('invalid_option');
      //$script->addGoto('hang_up');

      $script->addLabel('hang_up');
      $script->addHangup();
      break;
    case 'claro_intro_script':
      $script = new VoipScript('claro_intro_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $script->addSay(v("If this is an emergency, hang up and dial 9 1 1. "));
      $script->addSay(v("Welcome house cleaners, nannies, and caregivers! You have reached the Domestic Worker App - accessible by any kind of phone. "));

      if($voice == 'claro-portugese') {
        $script->addGosub('claro_main_menu_pt_script', $options);
      }
      else {
        $script->addGosub('claro_main_menu_script', $options);
      }
      $script->addReturn();
      break;
    case 'claro_main_menu_script':
      $script = new VoipScript('claro_main_menu_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('options_menu');

      $p_options_menu = v("Main Menu.
      To sign up to receive weekly updates by text, press 1.
      To learn about health and safety tips, press 2.
      To know your rights as a domestic worker, press 3.
      To learn about landmark victories for domestic workers, press 4.
      To learn about the Domestic Worker App, press 5.
      To be connected with the National Domestic Workers Alliance, press 6.
      To repeat this menu, press the star key.
      ");

      $input_options = array(
        '1' => 'sms_subscription',
        '2' => 'play_episodes',
        '3' => 'domestic',
        '4' => 'victories',
        '5' => 'get_information',
        '6' => 'domestic_workers_connect',
        '*' => 'repeat',
        //'#' => 'invalid_option',
        'i' => 'timeout',
        't' => 'timeout'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('play_episodes');
      $script->addGosub('claro_play_episodes_demo', $options);
      $script->addGoto('options_menu');

      $script->addLabel('get_information');
      $script->addGetInput(v('Here is recording for "ABOUT THE DOMESTIC WORKER APP"'), 1);
      $script->addGotoIf('options_menu', "^%input_digits");
      $script->addGoto('options_menu');

      $script->addLabel('sms_subscription');
      $options['signup_number'] = '%caller_number';
      $script->addGosub('claro_sms_subscription_menu', $options);
      $script->addGoto('options_menu');

      $script->addLabel('news');
      $script->addGosub('nannyvan_news_play_blogs_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('domestic');
      //$script->addSay(v('Your call will now be redirected to NDNS. '));
      //$script->addDial('+16466993989');
      $script->addGosub('claro_domestic_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('victories');
      //$script->addSay(v('Your call will now be redirected to NDNS. '));
      //$script->addDial('+16466993989');
      $script->addGosub('claro_victories_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('domestic_workers_connect');
      $script->addGosub('domestic_workers_connect_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('repeat');
      $script->addGoto('options_menu');

      $script->addLabel('timeout');
      $script->addReturn();
      break;
    case 'claro_play_episodes_demo':
      $script = new VoipScript('claro_play_episodes_demo');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('options_menu');
      $p_options_menu = v("Directory for Health and Safety Tips.
To learn about cleaning products that can harm - or even kill - you, press 1.
Preventing bed bug infestation, press 2.
Staying safe in style, press 3.
To repeat this menu, press the star key.
To return to the main menu, press the pound key.
");

      $input_options = array(
        '1' => 'da_lungs',
        '2' => 'bed_bug',
        '3' => 'fashion',
        '#' => 'go_back',
        'i' => 'hangup',
        't' => 'hangup'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('da_lungs');
      $options['story_id'] = 13;
      $script->addGosub('claro_play_story_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('bed_bug');
      $options['story_id'] = 42;
      $script->addGosub('claro_play_story_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('fashion');
      $options['story_id'] = 84;
      $script->addGosub('claro_play_story_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();

      $script->addLabel('hangup');
      $script->addHangup();
      break;
    case 'claro_main_menu_pt_script':
      $script = new VoipScript('claro_main_menu_pt_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('options_menu');

      $p_options_menu = v("Para saber dos seus direitos como trabalhador doméstico em massachusetts, presione a tecla 1.
Para entrar em contato com o centro do trabalhador brasileiro, pressione a tecla 2. 
Para entrar em contato com a procuradoria do estado de massachusetts, pressione a tecla 3. 
Para informações sobre o aplicativo dos trabalhadores domésticos pressione a tecla 4. 
Para episódios em inglês ou espanhol, pressione a tecla 5. 
Para repetir estas opções, pressione a tecla asterisco. 
      ");

      $input_options = array(
        '1' => 'massachusetts_episodes',
        '2' => 'brazilian_working_center',
        '3' => 'attorney_office',
        '4' => 'domestic',
        '5' => 'go_back',
        '*' => 'repeat',
        //'#' => 'invalid_option',
        'i' => 'hangup',
        't' => 'hangup'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('massachusetts_episodes');
      $script->addGosub('claro_play_episodes_pt', $options);
      $script->addGoto('options_menu');

      $script->addLabel('brazilian_working_center');
      //$script->addSay(v('Your call will now be redirected to New Day New Standard. '));
      $script->addDial('+16177838001');
      $script->addHangup();

      $script->addLabel('attorney_office');
      $script->addDial('+16177273465');
      $script->addHangup();

      $script->addLabel('domestic');
      $script->addGosub('about_domestic_pt', $options);
      $script->addGoto('options_menu');

      $script->addLabel('repeat');
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();

      $script->addLabel('hangup');
      $script->addHangup();
      break;
    case 'claro_play_episodes_pt':
      $script = new VoipScript('claro_play_episodes_pt');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('options_menu');
      $p_options_menu = v("PT EPISODES MENU");

      $end_key = '';
      $num_digits = 2;
      $script->addGetInput($p_options_menu, $num_digits, $end_key);

      /*$input_options = array(
        '1' => 'episode1',
        '2' => 'episode2',
        '3' => 'episode3',
        '4' => 'episode4',
        '5' => 'episode5',
        '6' => 'episode6',
        '7' => 'episode7',
        '8' => 'episode8',
        '9' => 'episode9',
        '10' => 'episode10',
        '11' => 'episode11',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');*/
      $script->addGotoIf('episode%input_digits', "^_claro_valid_pt_episode(%input_digits)");

      $script->addSay(v('Oops! That option is invalid. '));
      $script->addGoto('options_menu');

      $first_pt_story_nid = 2767;
      for($i=1;$i<=11;$i++) {
        $script->addLabel('episode'.$i);
        $options['story_id'] = $first_pt_story_nid;
        $script->addSay("You chosed episode ".$i);
        //$script->addGosub('claro_play_story_script', $options);
        $script->addGoto('options_menu');
        $first_pt_story_nid++;
      }

      $script->addLabel('go_back');
      $script->addReturn();
      break;
    case 'about_domestic_pt':
      $script = new VoipScript('about_domestic_pt');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $p_options_menu = v("Acessivel por qualquer tipo de telefone, o aplicativo dos trabalhadores domésticos presta...");

      $input_options = array(
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('go_back');
      $script->addReturn();
      break;
    case 'claro_play_story_script':
      $script = new VoipScript('claro_play_story_script');
      $story_id = $options['story_id'];
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $episode = claro_get_episode($story_id, $voice);
      if (!$episode) {
        $script->addSay("Invalid story id. ");
        $script->addLog("Invalid story id: $story_id. ");
        $script->addGoto('go_back');
      }
      else {
        $prompt = new VoipPrompt($episode['body']);
      }

      $script->addLabel('play_story');
      $p_episode_intro = v("You're about to listen to a new episode, but listen up.
If you want to start over, hear the list of episodes or the main menu, press any key at any time.");

      $p_episode_prompt[] = $p_episode_intro;
      $p_episode_prompt[] = $prompt;

      $script->addGetInput($p_episode_prompt, 1);

      $script->addGotoIf('episode_outro', "^%input_digits");

      $script->addLabel('episode_outro');
      $p_episode_outro = v('To repeat this episode, press 1.
      To hear other episodes press 2.
      To go back to the main menu, press the pound key. ');
      $input_options = array(
        '1' => 'repeat',
        '2' => 'go_back',
        '#' => 'go_back_main_menu',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_episode_outro, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('repeat');
      $script->addGoto('play_story');

      $script->addLabel('go_back');
      $script->addReturn();

      $script->addLabel('go_back_main_menu');
      $script->addGosub('claro_main_menu_script', $options);
      $script->addHangup();
      break;
    case 'claro_domestic_script':
      $script = new VoipScript('claro_domestic_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $script->addLabel('options_menu');
      $p_options_menu = v("Directory for Info about your rights in various states.
      To hear about the rights of domestic workers in New York, press 1.
      Massachusetts, press 2.
      California, press 3.
      To return to the main menu, press the pound key. ");

      $input_options = array(
        '1' => 'ny',
        '2' => 'coming_soon',
        '3' => 'california',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('ny');
      $script->addSay(v('Your call will now be redirected to New Day New Standard. '));
      $script->addDial('+16466993989');
      $script->addHangup();

      $script->addLabel('california');
      $script->addGosub('claro_california', $options);
      $script->addGoto('go_back');

      $script->addLabel('coming_soon');
      $script->addGosub('claro_coming_soon', $options);
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();

      break;
    case 'claro_california':
      $script = new VoipScript('claro_california');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('options_menu');
      $p_options_menu = v("Your rights as a domestic worker in California.
      For the definition of a domestic worker under California Law, press 1
      For minimum wage and overtime pay, press 2.
      For meal breaks, press 3.
      For info about meals and lodging or being fired, press 4.
      For info about workers comp, harassment, and immigration, press 5.
      To hear about the rights that domestic workers in California seek to achieve, press 6.
      For timesheets and recordkeeping, press 7.
      To repeat this menu, press the star key.
      To go back to the main menu for the Domestic Worker App, press the pound key.
");

      $input_options = array(
        '1' => 'domestic_worker',
        '2' => 'min_wage',
        '3' => 'meal',
        '4' => 'info_meals',
        '5' => 'info_workers',
        '6' => 'rights',
        '7' => 'timesheets',
        '*' => 'repeat',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );

      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('domestic_worker');
      $options['prompt'] = 'Here goes audio for the definition of a domestic worker under California Law';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('min_wage');
      $options['prompt'] = 'Here goes audio for minimum wage and overtime pay';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('meal');
      $options['prompt'] = 'Here goes audio for meal breaks';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('info_meals');
      $options['prompt'] = 'Here goes audio for info about meals and lodging or being fired';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('info_workers');
      $options['prompt'] = 'Here goes audio for info about workers comp, harassment, and immigration';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('rights');
      $options['prompt'] = 'Here goes audio for the rights that domestic workers in California seek to achieve';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('timesheets');
      $options['prompt'] = 'Here goes audio for timesheets and recordkeeping';
      $script->addGoSub('claro_california_play_script', $options);
      $script->addGoto('options_menu');

      $script->addLabel('repeat');
      $script->addGoto('play_story');

      $script->addLabel('go_back');
      $script->addReturn();
      break;

    case 'claro_california_play_script':
      $script = new VoipScript('claro_california_play_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $script->addLabel('play_story');
      $p_episode_intro = v("You're about to listen to a new episode, but listen up.
If you want to start over, hear the list of episodes or the main menu, press any key at any time.");

      $p_episode_prompt[] = $p_episode_intro;
      $p_episode_prompt[] = v($options['prompt']);

      $script->addGetInput($p_episode_prompt, 1);

      $script->addGotoIf('episode_outro', "^%input_digits");

      $script->addLabel('episode_outro');
      $p_episode_outro = v('To repeat this episode, press 1.
      To hear other episodes press 2.
      To go back to the main menu, press the pound key. ');
      $input_options = array(
        '1' => 'repeat',
        '2' => 'go_back',
        '#' => 'go_back_main_menu',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_episode_outro, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('repeat');
      $script->addGoto('play_story');

      $script->addLabel('go_back');
      $script->addReturn();

      $script->addLabel('go_back_main_menu');
      $script->addGosub('claro_main_menu_script', $options);
      $script->addHangup();
      break;


    case 'claro_victories_script':
      $script = new VoipScript('claro_domestic_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $script->addLabel('options_menu');
      $p_options_menu = v("To hear about a recent victory in Boston, Massachusetts, press 1.
To return to the main menu, press the pound key. ");

      $input_options = array(
        '1' => 'boston',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('boston');
      $episode = claro_get_episode(85, $voice); //Melnea
      if (!$episode) {
        $script->addSay("Invalid story id. ");
        //$script->addLog("Invalid story id: $story_id. ");
        $script->addGoto('go_back');
      }
      else {
        //$script->addSay($episode['body']);
        $script->addGetInput($episode['body'], 1);
        $script->addGotoIf('options_menu', "^%input_digits");
      }
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();
      break;
    case 'claro_coming_soon':
      $script = new VoipScript('claro_coming_soon');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $script->addSay(v('This part of the system is coming soon. '));
      $script->addReturn();
      break;
    case 'claro_sms_subscription_menu':
      $script = new VoipScript('claro_sms_subscription_menu');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);

      $signup_number = $options['signup_number'];
      $text = variable_get('claro_sms_help',
        'Welcome to The NannyVan App! To JOIN and receive weekly text messages, text the letter "J". To STOP receiving text messages, text "S". For HELP, text "H". Msg & data rates may apply.');

      $script->addSendText($text, $signup_number);

      $script->addLabel('options_menu');
      $p_options_menu = v("The Domestic Worker App offers weekly text messages for House cleaners, Childcare Providers, and caregivers about health and safety, history, rights, and more.
You should have already received a text message with instructions to confirm your subscription. So check your cell phone, and thanks for signing up!”
To repeat this message, press the star key.
To go back to the main menu, press the pound key. ");

      $input_options = array(
        '*' => 'repeat',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('repeat');
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();

      break;
    case 'domestic_workers_connect_script':
      $script = new VoipScript('domestic_workers_connect_script');
      $voice = $options['voice'];
      voipvoice_set_current_voice($voice);
      $p_options_menu = v("The National Domestic Workers Alliance (or N D W A) is the nation's
          leading voice for dignity and fairness for the millions of domestic workers in the United States.
          N D W A works for the respect, recognition, and inclusion in labor protections for domestic workers.
          The national alliance is powered by 44 affiliate organizations including our first local chapter
          in Atlanta of over 10,000 nannies, housekeepers, and caregivers for the elderly in 25 cities and
          17 states. To speak with a person at the main office, call them at (352) 630-0862 or dial the star
          key now. To go back to the main menu, press the pound key.");

      $input_options = array(
        //'1' => 'confirm',
        '*' => 'call_ndwa',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $p_invalid_msg = v('Oops! That option is invalid. ');
      $script->addRunIvrMenu($p_options_menu, $input_options, $p_invalid_msg);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('call_ndwa');
      $script->addSay(v('Your call will now be transferred to N D W A. '));
      $script->addDial('+13526390862');
      $script->addGoto('options_menu');

      $script->addLabel('go_back');
      $script->addReturn();

      break;
    
    case 'claro_sms_handler_script':
      //Claro VoIP script for Text channel
      $script = new VoipScript('claro_sms_handler_script');
      // log the sms
      $log_msg = t("SMS from %caller_number on @date (id: %call_id)",
        array('@date' => format_date(time(), 'custom', 'D, m/d/Y - H:i:s')));
      $script->addLog($log_msg, 'MDT');

      // below was the only way to avoid problems with special characters in the text message
      $options['text'] = '%inbound_text_contents';
      $options['caller_number'] = '%caller_number';
      $options['dest_number'] = '%dest_number';
      $options['call_id'] = '%cid';
      $options['source'] = '%source';
      $script->addGosub('claro_sms_process_request_script', $options);
      break;
    case 'claro_sms_process_request_script':
      //Helper script to process SMS request
      $script = new VoipScript('claro_sms_process_request_script');

      $text = _claro_sms_process_request($options['text'], $options['caller_number'], $options['dest_number'], $options['call_id']);

      if ($text) {
        $script->addSendText($text);
        $script->addLog("sending $text");
      }

      $script->addHangup();
      break;
  }

  return $script;
}

/**
 * Process incoming SMS messages
 */
function _claro_sms_process_request($msg, $caller_number, $dest_number, $call_id) {
  watchdog('claro', "In _claro_sms_process_request with $msg, $caller_number, $dest_number");
  $caller_number = VoipCall::NormalizeNumber($caller_number);
  $help_msg = variable_get('claro_sms_help',
    'Welcome to The NannyVan App! To JOIN and receive weekly text messages, text the letter "J". To STOP receiving text messages, text "S". For HELP, text "H". Msg & data rates may apply.');
  $msg = trim($msg);

  $phone_number = claro_get_phone_number($caller_number);

  if (!$phone_number) {
    //Number doesn't exists
    $cmd = 'J';
  }
  else {
    if ($msg) {
      $request = explode(" ", $msg);
      $cmd = strtoupper($request[0]);

      if(!$cmd == 'Y' && !$phone_number->field_opt_in['und'][0]['value']) {
        watchdog('debug', "received $cmd for unopted contact. Returning optin message");
        $ret = variable_get('claro_opt_message',
          '[Company] would like to send you SMS reminders @ this number. X/week. HELP for help. STOP to end. Msg&data rates may apply. Reply KEYWORD to confirm today.');
        return $ret;
      }
    }
  }

  if ($cmd == 'Y') {
    if ($phone_number) {
      $phone_number->field_opt_in['und'][0]['value'] = 1; //verify
      node_save($phone_number);
      $ret = variable_get('claro_sms_welcome', 'Welcome to NannyVan App');
    }
    else {
      $ret = t('Phone number not recognised.');
    }
  }
  else {
    if ($cmd == 'S') {
      //Stop getting messages
      $nids = VoipNumber::getNidsByNumber($caller_number);
      if ($nid = reset($nids)) {
        node_delete($nid);
        $ret = variable_get('claro_sms_unsubscribe', 'You\'ve unsubscribed from the Domestic Worker App and should not receive further messages. Should you choose to subscribe again, text any letter to the following number: (347) 967-5500');
      }
      else {
        $ret = variable_get('claro_sms_not_in_list', 'Number not in the list.');
      }
    }
    else {
      if ($cmd == 'J') {
        if ($phone_number) {
          //Number already exists
          $ret = variable_get('claro_sms_unknown', 'Number already registered.');
        }
        else {
          $node->type = 'phone_number';
          $node->title = $caller_number;
          $node->language = 'und';
          $node->status = 1;
          $node->field_opt_in['und'][0]['value'] = 1; //verify
          $node->field_phone_number['und'][0]['vnid'] = $caller_number;
          $language = claro_number_to_lang($dest_number);
          $node->field_language['und'][0]['value'] = $language;
          // create the node
          node_save($node);
          //@todo: verify number
          if ($node->nid) {
            $ret = variable_get('claro_sms_welcome', 'Welcome to NannyVan App');
          }
          else {
            watchdog('claro', 'Error saving phone number: ' . print_r($node, TRUE), array(), WATCHDOG_ERROR);
            $ret = t('System error. Please try again later.');
          }
        }
      }
      else {
        if ($cmd == 'H') {
          $ret = $help_msg;
        }
        else {
          $ret = variable_get('claro_sms_unknown', 'Unknown command.') . ' ' . $help_msg;
        }
      }
    }
  }

  return $ret;
}

function claro_get_episode($nid, $voice) {
  global $user;
  $n = node_load($nid);
  if (!$n) {
    return NULL;
  }

  $voipvoice = voipvoice_get_voice($voice);
  $lang = $voipvoice['language'];
  //Switch the user, because translation_node_get_translations will not return any nodes since VoIP Drupal script runs as anon user
  $current_user = $user;
  $user = user_load(1);
  $translations = translation_node_get_translations($n->tnid);
  $translated_node = (isset($translations[$lang]) ? node_load($translations[$lang]->nid) : $n);
  //switch back to current user
  $user = $current_user;

  $episode = array();
  $episode['nid'] = $translated_node->nid;
  $episode['changed'] = $translated_node->field_episode_date['und'][0]['value'];

  if ($translated_node->field_audio['und'][0]) {
    $audio_url = file_create_url($translated_node->field_audio['und'][0]['uri']);
    $episode['body'] = $audio_url;
  }

  if (!$episode['body']) {
    //If no audio then load text version
    $episode['body'] = $translated_node->body['und'][0]['value'];
  }

  return $episode;
}

/*
* Returns array of claro nodes
*/
/*
function _claro_get_episodes() {
  $episodes = array();

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'episode')
    ->propertyCondition('status', 1)
    ->fieldOrderBy('field_episode_date', 'value', 'DESC')
    ->addMetaData('account', user_load(1)); // Run the query as user 1.
  $result = $query->execute();
  if (isset($result['node'])) {
    $episode_nids = array_keys($result['node']);
    $episode_items = node_load_multiple($episode_nids);
    foreach ($episode_items as $n) {
      $episode = array();
      $episode['nid'] = $n->nid;
      $episode['changed'] = $n->field_episode_date['und'][0]['value'];

      $field_audio = $n->field_audio;
      if ($field_audio['und'][0]) {
        $audio_url = file_create_url($field_audio['und'][0]['uri']);
        $episode['body'] = $audio_url;
      }

      if (!$episode['body']) {
        //If no audio then load text version
        $episode['body'] = $n->body['und'][0]['value'];
      }

      $episodes[] = $episode;
    }
  }

  return $episodes;
}
*/

function _claro_valid_pt_episode($option) {
  if($option >= 1 && $option <= 11) {
    return TRUE;
  }
  return FALSE;
}