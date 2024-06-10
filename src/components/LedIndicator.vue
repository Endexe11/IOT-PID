<template>
  <div :class="ledClass" :style="ledStyle"></div>
</template>

<script>
export default {
  name: 'LedIndicator',
  props: {
    pwm: {
      type: Number,
      required: true
    }
  },
  computed: {
    ledClass() {
      //console.log('LED PWM:', this.pwm);
      return {
        'led': true
      };
    },
    ledStyle() {
      return {
        opacity: this.pwm / 1024, // Asumiendo que el valor de PWM está entre 0 y 1024
        backgroundColor: this.pwm > 0 ? 'green' : 'gray'
      };
    }
  },
  watch: {
    pwm(newVal) {
      //console.log('pwm changed:', newVal);
    }
  }
};
</script>

<style scoped>
.led {
  width: 100px;
  height: 100px;
  border-radius: 50%;
  margin: 10px;
  border: 7px solid rgb(0, 0, 0); /* Margen visual para resaltar el LED */
}
</style>



  