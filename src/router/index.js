// src/router/index.js
import { createRouter, createWebHistory } from 'vue-router';
import Login from '@/components/LoginUser.vue';
import Dashboard from '@/components/DashboardUser.vue';
import Experiment from '@/components/ExperimentUser.vue';
import LineChart from '@/components/LineChart.vue';
import Comparison from '@/components/ComparationExperiment.vue';
import { auth } from '@/firebaseConfig';

const routes = [
  { 
    path: '/', 
    component: Login 
  },
  { 
    path: '/dashboard', 
    component: Dashboard, 
    meta: { requiresAuth: true } 
  },
  { 
    path: '/experiment/:id', 
    component: Experiment, 
    meta: { requiresAuth: true },
    props: true,
  },
  { 
    path: '/linechart/:id', 
    component: LineChart, 
    meta: { requiresAuth: true }, 
    props: true,
  },
  { 
    path: '/comparison', 
    component: Comparison, meta: 
    { requiresAuth: true }, 
    props: true, },

  {
    path: '/:catchAll(.*)',
    redirect: '/experiment/1'
  }
];

const router = createRouter({
  history: createWebHistory(),
  routes
});

router.beforeEach((to, from, next) => {
  const requiresAuth = to.matched.some(record => record.meta.requiresAuth);
  const isAuthenticated = auth.currentUser;

  if (requiresAuth && !isAuthenticated) {
    next('/');
  } else {
    next();
  }
});

export default router;


