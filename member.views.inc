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
  $data['member']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
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
