<?php

/**
 * Search service class using an RDF store server.
 */
class RdfIndexerVirtuosoService extends SearchApiAbstractService {

  public function __construct(SearchApiServer $server) {
    parent::__construct($server);
    $this->setAuth(NULL);
  }
  
  // It's unknown if instances of this Service persist 
  // so the Auth information is may not be available 
  // for subsequent requests. For now, we include the 
  // use of variable_set/variable_get to assist until
  // this can be resolved
  private $auth;
  
  public function setAuth($_auth){
    $this->auth = $_auth;
    variable_set($this->getUniqueName(),$_auth);
  }
  
  public function getAuth(){
    return !empty($this->auth) 
      ? $this->auth 
      : variable_get($this->getUniqueName(), array());
  }
  
  /*
   * Generate a unique string based on the machine name of the server
   */
  private function getUniqueName(){
    return 'rdf_virtuoso_'.$this->server->machine_name;
  }

  /**
   * Form callback. Might be called on an uninitialized object - in this case,
   * the form is for configuring a newly created server.
   *
   * Returns an empty form by default.
   *
   * @return array
   *   A form array for setting service-specific options.
   */
  public function configurationForm(array $form, array &$form_state) {
    global $base_url;
    
    $settings = isset($this->server->options) ? $this->server->options : array();
    $form['url'] = array(
      '#type' => 'textfield',
      '#title' => t('Virtuoso SPARQL Endpoint URL'),
      '#description' => t('The Virtuoso SPARQL Endpoint URL where data will be inserted and deleted via basic authentication. Typically, http://example.com:8890/sparql-auth'),
      '#default_value' => isset($settings['url']) ? $settings['url'] : '',
      '#required' => TRUE,
    );
    $form['graph'] = array(
      '#type' => 'textfield',
      '#title' => t('Graph URI'),
      '#description' => t('The URI of the graph where data will be inserted and deleted.'),
      '#default_value' => isset($settings['graph']) ? $settings['graph'] : $base_url,
      '#required' => TRUE,
    );
    $form['username'] = array(
      '#type' => 'textfield',
      '#title' => t('Username'),
      '#description' => t('The username of the Virtuoso account with SPARQL/Update privileges'),
      '#default_value' => isset($settings['username']) ? $settings['username'] : '',
      '#required' => TRUE,
    );   
    //assume that 'Administer Search API' permission is valid enough to view the password 
    $form['credential'] = array(
      '#type' => 'textfield',
      '#title' => t('Credential'),
      '#description' => t('The credential of the Virtuoso account with SPARQL/Update privileges'),
      '#default_value' => isset($settings['credential']) ? $settings['credential'] : '',
      '#required' => TRUE,
    );    
    return $form;
  }
  
  /**
   * Validation callback for the form returned by configurationForm().
   *
   * $form_state['server'] will contain the server that is created or edited.
   * Use form_error() to flag errors on form elements.
   *
   * @param array $form
   *   The form returned by configurationForm().
   * @param array $values
   *   The part of the $form_state['values'] array corresponding to this form.
   * @param array $form_state
   *   The complete form state.
   */
  public function configurationFormValidate(array $form, array &$values, array &$form_state){
  
  }

  /**
   * View this server's settings. Output can be HTML or a render array, a <dl>
   * listing all relevant settings is preferred.
   *
   * The default implementation does a crude output as a definition list, with
   * option names taken from the configuration form.
   */
  public function viewSettings() {
    $settings = isset($this->server->options) ? $this->server->options : array();
    if( !empty($settings) ){
      require './includes/password.inc';
      $output = "<dl>\n  <dt>";
      $output .= t('Virtuoso SPARQL endpoint');
      $output .= "</dt>\n  <dd>";
      $output .= l($settings['url'], $settings['url']);
      $output .= "</dd>\n  <dt>";
      $output .= t('Graph URI');
      $output .= "</dt>\n<dd>";
      $output .= $settings['graph'];
      $output .= "</dd>\n  <dt>";
      $output .= t('Username');
      $output .= "</dt>\n  <dd>";
      $output .= $settings['username'];
      $output .= "</dd>\n  <dt>";
      $output .= t('Credential');
      $output .= "</dt>\n <dd>";
      $output .= user_hash_password($settings['credential']);
      $output .= "</dd>\n</dl>";
      return $output;
    } else {
      return 'Virtuoso has not been configured.';
    }
    
  }

  /**
   * Notifies this server that it is about to be deleted from the database and
   * should therefore clean up, if appropriate.
   *
   * Note that you shouldn't call the server's save() method, or any
   * methods that might do that, from inside of this method as the server isn't
   * present in the database anymore at this point.
   *
   * By default, deletes all indexes from this server.
   */
  public function preDelete() {
    // Only react on real deletes, not on reverts.
    // @see https://drupal.org/node/1414078
    // This method could be removed once the above issue is fixed.
    if ($this->server->hasStatus(ENTITY_IN_CODE)) {
      return;
    }
    $indexes = search_api_index_load_multiple(FALSE, array('server' => $this->server->machine_name));
    foreach ($indexes as $index) {
      $this->removeIndex($index);
    }
  }

  /**
   * Add a new index to this server.
   *
   * @param SearchApiIndex $index
   *   The index to add.
   */
  public function addIndex(SearchApiIndex $index) {
    if (module_exists('search_api_multi') && module_exists('search_api_views')) {
      views_invalidate_cache();
    }
  }

  /**
   * Notify the server that the indexed field settings for the index have
   * changed.
   * If any user action is necessary as a result of this, the method should
   * use drupal_set_message() to notify the user.
   *
   * @param SearchApiIndex $index
   *   The updated index.
   *
   * @return
   *   TRUE, if this change affected the server in any way that forces it to
   *   re-index the content. FALSE otherwise.
   */
  public function fieldsUpdated(SearchApiIndex $index) {
    if (module_exists('search_api_multi') && module_exists('search_api_views')) {
      views_invalidate_cache();
    }
    return TRUE;
  }

  /**
   * Index the specified items.
   *
   * @param SearchApiIndex $index
   *   The search index for which items should be indexed.
   * @param array $items
   *   An array of items to be indexed, keyed by their id. The values are
   *   associative arrays of the fields to be stored, where each field is an
   *   array with the following keys:
   *   - type: One of the data types recognized by the Search API, or the
   *     special type "tokens" for fulltext fields.
   *   - original_type: The original type of the property, as defined by the
   *     datasource controller for the index's item type.
   *   - value: The value to index.
   *
   *   The special field "search_api_language" contains the item's language and
   *   should always be indexed.
   *
   *   The value of fields with the "tokens" type is an array of tokens. Each
   *   token is an array containing the following keys:
   *   - value: The word that the token represents.
   *   - score: A score for the importance of that word.
   *
   * @return array
   *   An array of the ids of all items that were successfully indexed.
   *
   * @throws SearchApiException
   *   If indexing was prevented by a fundamental configuration error.
   */
  public function indexItems(SearchApiIndex $index, array $items) {
    
    $ret = array();
    foreach ($items as $id => $item) {
      try {
        // Builds an RDF resource for the entity.
        $rdf = rdfx_get_rdf_model($index->item_type, $id);
        
        //insert data into Virtuoso 
        $this->insertItem($rdf);
        
        $ret[] = $id;
      }
      catch (Exception $e) {
        watchdog_exception('rdf_virtuoso', $e, "%type while indexing @type with ID @id: !message in %function (line %line of %file).", array('@type' => $index->item_type, '@id' => $id), WATCHDOG_WARNING);
      }
    }
    
    return $ret;
  }

  /**
   * Delete items from an index on this server.
   *
   * Might be either used to delete some items (given by their ids) from a
   * specified index, or all items from that index, or all items from all
   * indexes on this server.
   *
   * @param $ids
   *   Either an array containing the ids of the items that should be deleted,
   *   or 'all' if all items should be deleted. Other formats might be
   *   recognized by implementing classes, but these are not standardized.
   * @param SearchApiIndex $index
   *   The index from which items should be deleted, or NULL if all indexes on
   *   this server should be cleared (then, $ids has to be 'all').
   */
  public function deleteItems($ids = 'all', SearchApiIndex $index = NULL) {
    
    try {
      // Emtpy the local store.
      if ($ids === 'all') {
        //clear the entire graph
        $this->clearGraph();
      }
      elseif (is_array($ids)) {
        // Contructs the URI of the graph for each entity ID and deletes it.
        foreach($ids as $id) {
          $entity = entity_load_single($index->item_type, $id);
          $uri = rdfx_resource_uri($index->item_type, $entity);
          
          //delete data from Virtuoso at graph $settings['graph'];
          $this->deleteItem($uri);
        }
      }
    }
    catch(Exception $e) {
      watchdog_exception('rdf_virtuoso', $e, '%type while deleting items from server @server: !message in %function (line %line of %file).', array('@server' => $this->server->name));
    }
  }

  public function search(SearchApiQueryInterface $query) {
    throw new SearchApiException(t('The RDF indexer service does not support search. Please query the SPARQL endpoint directly if the RDF store provide such service.'));
  }
  
  /*
   * Insert the given RDF into the triplestore
   * 
   * @param $rdf
   *    The rdfx_get_rdf_model object to insert
   * @param $options
   *    Array provided to expansion of the function
   *    Currently, it supports returning the query string
   */
  protected function insertItem($rdf, array $options = array()){
    $graph =  $this->server->options['graph'];
    $query = 'INSERT DATA INTO GRAPH <'.$graph.'> { ';
    foreach($rdf->index as $subject => $triples){
      foreach($triples as $predicate => $value){
        foreach($value as $triple){
          $object = '';
          if( $triple['type'] == 'uri' ){
            $object .= '<'.$triple['value'].'>';
          } else {
            if( !empty($triple['datatype']) ){
              $object .= '"'.$triple['value'].'"^^'.$triple['datatype'];
            } else {
              $object .= '"'.$this->prepareString($triple['value']).'"';
            }
          }
          $query .= ' <'.$subject.'> <'.$predicate.'> '.$object.' . ';
        }
      }
    }
    $query .= '}';
    
    if( isset($options['display']) ){
      return htmlspecialchars($query);
    } else {
      return $this->updateStore($query);
    }
  }
  
  /*
   * Delete the given URI from the triplestore
   * 
   * @param $uri
   *    A uri of a subject in the triplestore to remove
   * @param $options
   *    Array provided to expansion of the function
   *    Currently, it supports returning the query string
   */
  protected function deleteItem($uri, array $options = array()){
    $graph =  $this->server->options['graph'];
    $query = 'WITH <'.$graph.'> DELETE { ?s ?p ?o } WHERE { ?s ?p ?o FILTER (?s = <'.$uri.'>)}';
    if( isset($options['display']) ){
      return htmlspecialchars($query);
    } else {
      return $this->updateStore($query);
    }
  }
  
  /*
   * Clear the triplestore graph
   *
   * @param $options
   *    Array provided to expansion of the function
   *    Currently, it supports returning the query string
   */
  protected function clearGraph(array $options = array()){
    $graph =  $this->server->options['graph'];
    $query = 'CLEAR GRAPH <'.$graph.'>';
    if( isset($options['display']) ){
      return htmlspecialchars($query);
    } else {
      return $this->updateStore($query);
    }
  }
  
  /*
   * Connect to the triplestore and run a query
   *
   * Virtuoso at the /sparql-auth URL implements Digest Authentication
   */
  protected function updateStore($query){
    $url = $this->getTriplestoreURL() .'?default-graph-uri=&format=text%2Fhtml&timeout=0&debug=on&query='.urlencode($query);
    
    $headers = $this->getAuth();
    $response = drupal_http_request($url, array('headers' => $headers));
    if( $response->code == 401 ){
    
      $uri = parse_url($this->getTriplestoreURL());
      $this->authenticate($response->headers['www-authenticate'], $uri['path']);
      $headers = $this->getAuth();
      $response = drupal_http_request($url,  array('headers' => $headers));
      if( $response->code != 200 ){
        watchdog('rdf_virtuoso', $response->error. '%error (%code) Could not authenticate to the triplestore', array('%error' => $uri['path'], '%code' => $response->code), WATCHDOG_ERROR, $url);
        return FALSE;
      }
    } elseif($response->code != 200 ) {
      watchdog('rdf_virtuoso', $response->error. '%error (%code) Could not connect to the triplestore', array('%error' => $uri['path'], '%code' => $response->code), WATCHDOG_ERROR, $url);
      return FALSE;
    }
    return $response->data;
  }
  
  /*
   * Build the Authorization header for Virtuoso
   *
   * @param $digest
   *    The digest information passed from Virtuoso via the WWW-Authenticate header
   * @param $uri
   *    The uri path at which the original request was made
   */
  protected function authenticate($digest, $uri){
    $value = explode(' ', $digest, 2);
    $data = array();
    $parts = explode(", ", $value[1]);
    foreach ($parts as $element) {
    	$bits = explode("=", $element);
    	$data[$bits[0]] = str_replace('"','', $bits[1]);
    }
    
    if( $data['qop'] == 'auth'){
      $cnonce = time();
      $ncvalue = '00000001';
      $noncebit = $data['nonce'].":".$ncvalue.":".$cnonce.":auth:".md5("GET:".$uri);
      $A = md5($this->server->options['username'].":".$data['realm'].":".$this->server->options['credential']);
      $respdig = md5("$A:$noncebit");
      $auth_header = 'Digest username="'.$this->server->options['username'].'", realm="'.$data['realm'].'", nonce="'.$data['nonce'].'",';
      $auth_header .= ' uri="'.$uri.'", algorithm='.$data['algorithm'].', response="'.$respdig.'", opaque="'.$data['opaque'].'", qop='.$data['qop'].', nc='.$ncvalue.', cnonce="'.$cnonce.'"';
      $this->setAuth(array('Authorization' => $auth_header));
    } else {
      drupal_set_message('Cannot authenticate against the triplestore: '.$this->getTriplestoreURL());
    }
  }
  
  /*
   * Clean up literal string values for processing
   */
  protected function prepareString($object = FALSE){
    if( !empty($object) ){
      //strip out control characters
      if( !ctype_print($object) ){
        $object = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $object);
      }
      //escape double quotes
      $object = str_replace( "\"", "\\\"", $object );
    }
    return $object;
  }
  
  /*
   * Return the triplestore URL
   */
  public function getTriplestoreURL(){
    return $this->server->options['url'];
  }
  
  /*
   * Set a drupal message with the insert/delete queries 
   * or the response data for a given entity type and entity ID
   *
   * @param $entity_type
   *    The type of entity to test
   *
   * @param $id
   *    The entity ID
   *
   * @param $display
   *    A boolean flag that if TRUE just displays the queries. 
   *    If FALSE, the queries are executed and the response data 
   *    is displayed instead
   *
   *  @param $toggle
   *    A string where 'insert' runs just the insert routine,
   *    'delete' runs just the delete routine, and
   *    'both' runs both insert and delete routines
   */
  public function testQueries($entity_type, $id, $display = TRUE, $toggle = 'both'){
    $options = $display ? array('display' => $display) : array();
    $rdf= rdfx_get_rdf_model($entity_type, $id);
    
    if( $toggle == 'both' || $toggle == 'insert'){
      drupal_set_message($this->insertItem($rdf, $options));
    }
    if( $toggle == 'both' || $toggle == 'delete'){
      drupal_set_message($this->deleteItem($rdf->uri, $options));
    }
  }
}
