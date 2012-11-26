<?php
/**
 * @file
 * Expose member data to Views.
 */

/**
 * Implements hook_views_data().
 */
function member_views_data() {
  $data = array();

  $data['member']['table']['group']  = t('Member');

  $data['member']['table']['base'] = array(
    'field' => 'member_id',
    'title' => t('Members'),
    'help' => t('Member information.'),
    'access query tag' => 'member_access',
    'defaults' => array(
      'field' => 'member_id',
    ),
  );
  $data['member']['table']['entity type'] = 'member';

  // Join to the User table ({users}).
  $data['users']['table']['join']['member'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );
  $data['member']['table']['join']['users'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );

  // Expose the member ID.
  $data['member']['member_id'] = array(
    'title' => t('Member ID'),
    'help' => t('The unique internal identifier of the member.'),
    'field' => array(
      'field' => 'member_id',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  
  // Expose the member's user ID.
  $data['member']['uid'] = array(
    'title' => t('Uid'),
    'help' => t("The owner's user ID."),
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // display this field in the summary
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'title' => t('User'),
      'help' => t("Relate this member to its associated user account"),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'field' => 'uid',
      'label' => t('Member\'s user account'),
    ),
  );

  // Expose the member UUID.
  $data['member']['uuid'] = array(
    'title' => t('Member UUID'),
    'help' => t('The universally unique identifier of the member.'),
    'field' => array(
      'field' => 'uuid',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
