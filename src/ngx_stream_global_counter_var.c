#include <ngx_stream.h>
#include "ngx_global_counter_var.h"
#include <assert.h>


static void set_varval(ngx_stream_variable_value_t *v, u_char *data, size_t len) {
  v->valid = 1;
  v->no_cacheable = 1;
  v->not_found = 0;
  v->len = len;
  v->data = data;
}

static ngx_int_t ngx_stream_global_counter_var_get_handler(ngx_stream_session_t *sesh, ngx_stream_variable_value_t *v, uintptr_t data) {
  u_char                *buf = ngx_stream_get_module_ctx(sesh, ngx_stream_global_counter_var_module);
  if(!buf) {
    buf = ngx_palloc(sesh->connection->pool, NGX_INT_T_LEN + 1);
    if(!buf) {
      v->valid = 0;
      return NGX_ERROR;
    }
    ngx_stream_set_ctx(sesh, buf, ngx_stream_global_counter_var_module);
  }
  ngx_atomic_uint_t     current_val = ngx_atomic_fetch_add(ngx_global_counter, 1);
  
  size_t len = ngx_sprintf(buf, "%ui", current_val) - buf;
  set_varval(v, buf, len);
  
  return NGX_OK;
}

static ngx_int_t ngx_stream_global_counter_var_init_preconfig(ngx_conf_t *cf) {
  ngx_stream_variable_t         *var;
  ngx_str_t                      name = ngx_string("global_counter");
  var = ngx_stream_add_variable(cf, &name, NGX_STREAM_VAR_NOCACHEABLE|NGX_STREAM_VAR_WEAK);
  if(!var) {
    return NGX_ERROR;
  }
  var->get_handler = ngx_stream_global_counter_var_get_handler;
  return NGX_OK;
}

static ngx_int_t ngx_stream_global_counter_var_init_postconfig(ngx_conf_t *cf) {
  
  return NGX_OK;
}
static ngx_int_t ngx_stream_global_counter_var_init_module(ngx_cycle_t *cycle) {
  
  return NGX_OK;
}


static ngx_int_t ngx_stream_global_counter_var_init_worker(ngx_cycle_t *cycle) {  
  return NGX_OK;
}

static void ngx_stream_global_counter_var_exit_worker(ngx_cycle_t *cycle) { 
  
}

static void ngx_stream_global_counter_var_exit_master(ngx_cycle_t *cycle) {
  
}

static ngx_command_t ngx_stream_global_counter_var_commands[] = {
  ngx_null_command
};

static ngx_stream_module_t ngx_stream_global_counter_var_ctx = {
  ngx_stream_global_counter_var_init_preconfig,  /* preconfiguration */
  ngx_stream_global_counter_var_init_postconfig, /* postconfiguration */
  NULL,                          /* create main configuration */
  NULL,                          /* init main configuration */
  NULL,                          /* create server configuration */
  NULL,                          /* merge server configuration */
};

ngx_module_t ngx_stream_global_counter_var_module = {
  NGX_MODULE_V1,
  &ngx_stream_global_counter_var_ctx,         /* module context */
  ngx_stream_global_counter_var_commands,     /* module directives */
  NGX_STREAM_MODULE,                          /* module type */
  NULL,                                       /* init master */
  ngx_stream_global_counter_var_init_module,  /* init module */
  ngx_stream_global_counter_var_init_worker,  /* init process */
  NULL,                                       /* init thread */
  NULL,                                       /* exit thread */
  ngx_stream_global_counter_var_exit_worker,  /* exit process */
  ngx_stream_global_counter_var_exit_master,  /* exit master */
  NGX_MODULE_V1_PADDING
};
