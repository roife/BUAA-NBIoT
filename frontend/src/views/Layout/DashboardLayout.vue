<template>
  <div class="wrapper">
    <div class="main-content">
      <dashboard-navbar :type="$route.meta.navbarType"></dashboard-navbar>

      <div @click="$sidebar.displaySidebar(false)">
        <!-- your content here -->
        <router-view></router-view>
      </div>
    </div>
  </div>
</template>
<script>
  /* eslint-disable no-new */
  import PerfectScrollbar from 'perfect-scrollbar';
  import 'perfect-scrollbar/css/perfect-scrollbar.css';

  function hasElement(className) {
    return document.getElementsByClassName(className).length > 0;
  }

  function initScrollbar(className) {
    if (hasElement(className)) {
      new PerfectScrollbar(`.${className}`);
    } else {
      // try to init it later in case this component is loaded async
      setTimeout(() => {
        initScrollbar(className);
      }, 100);
    }
  }

  import DashboardNavbar from './DashboardNavbar.vue';
  import DashboardContent from './Content.vue';

  export default {
    components: {
      DashboardNavbar,
      DashboardContent,
    },
    methods: {
      initScrollbar() {
        let isWindows = navigator.platform.startsWith('Win');
        if (isWindows) {
          initScrollbar('sidenav');
        }
      }
    },
    mounted() {
      this.initScrollbar()
    }
  };
</script>
<style lang="scss">
</style>
