<template>
  <card class="card-stats" :show-footer-line="true">
    <b-row>
      <b-col>
        <slot>
          <h5 class="card-title text-uppercase text-muted mb-0" v-if="title">{{title}}</h5>
          <span class="h2 font-weight-bold mb-0" v-if="subTitle">{{subTitle}}</span>
        </slot>
      </b-col>

      <b-col cols="auto" v-if="$slots.icon || icon">
        <slot name="icon">
          <div class="icon icon-shape text-white rounded-circle shadow"
               :class="[`bg-${type}`, iconClasses]">
            <b-icon :icon="`${icon}`"></b-icon>
          </div>
        </slot>
      </b-col>
    </b-row>
  </card>
</template>
<script>
  import Card from './Card.vue';

  export default {
    name: 'stats-card',
    components: {
      Card
    },
    props: {
      type: {
        type: String,
        default: 'primary'
      },
      icon: String,
      title: String,
      subTitle: String,
      iconClasses: [String, Array]
    }
  };
</script>
<style></style>
