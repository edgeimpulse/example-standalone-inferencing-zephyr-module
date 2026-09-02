/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(__has_include)
    #if __has_include(<zephyr/version.h>)
        #include <zephyr/version.h>
    #elif __has_include(<version.h>)
        #include <version.h>
    #else
        #error "Could not find Zephyr version header"
    #endif
#else
    #include <zephyr/version.h> //assume latest version of Zephyr if __has_include is not available
#endif

#if (KERNEL_VERSION_MAJOR > 3) || ((KERNEL_VERSION_MAJOR == 3) && (KERNEL_VERSION_MINOR >= 1))
#include <zephyr/kernel.h>
#else
#include <zephyr.h>
#endif
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#ifdef EI_NORDIC
#include <nrfx_clock.h>
#endif

static const float features[] = {
    // copy raw features here (for example from the 'Live classification' page)
    // see https://docs.edgeimpulse.com/docs/running-your-impulse-locally-zephyr
    -1.3300, -1.4900, 10.0500, 0.5800, 2.0500, 10.2700, 2.9800, -0.6200, 9.8400, -3.9300, 3.9800, 9.6000, -3.9300, 3.9800, 9.6000, -1.9600, -1.8700, 10.5000, 0.8800, 0.3500, 9.8600, 2.1400, 0.2700, 9.4200, 0.7400, 0.1100, 9.9400, 1.3700, 0.5000, 9.5900, 1.6700, 1.2800, 8.7900, 1.6700, 1.2800, 8.7900, 1.6700, 1.3000, 7.5500, 1.0900, 0.9300, 10.1900, 0.7000, 0.5900, 9.6700, 0.1200, 0.7000, 9.2800, -0.0300, 0.8700, 9.4400, 0.2000, 1.8400, 8.7700, 0.2000, 1.8400, 8.7700, -0.2600, 1.4100, 9.8600, 0.6800, 2.0800, 8.5300, 0.2600, 1.2700, 8.9900, -0.1900, 0.0200, 9.6900, 0.6600, 0.2800, 9.4500, 0.2800, 0.3900, 9.8700, 0.2800, 0.3900, 9.8700, -0.1900, 0.1900, 9.4600, -0.5700, -0.7200, 9.5200, -1.6300, -1.8900, 9.3300, -1.9600, -2.2300, 10.3100, -1.9900, -2.2800, 9.3500, -0.8800, -0.8200, 10.1800, -0.8800, -0.8200, 10.1800, -0.6700, 0.2200, 9.7800, -0.3700, 0.2100, 9.6100, -1.5100, -1.3600, 10.1400, -1.3400, -3.0700, 10.0900, -1.4200, -2.4500, 9.4100, -1.4200, -2.4500, 9.4100, -1.0500, -1.5000, 10.2400, -0.5100, 0.3200, 9.7300, -0.0500, 0.7900, 10.0600, 0.3900, 0.0700, 10.1600, 0.2100, -0.5400, 10.1100, 0.3800, -1.5100, 9.9300, 0.3800, -1.5100, 9.9300, -0.4100, -0.8200, 9.7400, -0.1700, -0.9100, 9.9100, -0.2400, 0.1000, 10.1300, -0.0800, -0.3600, 10.0300, 0.3900, -0.8200, 9.4000, 0.6500, -0.6200, 10.2800, 0.6500, -0.6200, 10.2800, 0.7800, -0.3700, 8.5800, 1.2800, -0.1700, 8.9600, 1.7000, 0.2600, 10.2200, 2.0800, 0.3400, 10.3100, 1.7200, 0.6000, 9.5200, 1.5400, 1.2400, 9.8500, 1.5400, 1.2400, 9.8500, 1.2900, 2.4500, 9.9500, 1.8500, 2.9300, 9.6300, 1.7100, 3.0600, 9.6400, 1.3800, 2.9300, 9.7600, 0.7400, 2.2500, 8.7200, 0.3300, 1.5900, 7.5100, 0.3300, 1.5900, 7.5100, 0.3000, 1.3000, 8.3100, -0.2100, 1.6500, 8.6300, -0.1500, 2.0600, 8.8200, -0.8600, 1.9100, 9.7700, -1.0200, 1.1800, 9.3900, -0.8100, 1.1100, 10.0200, -0.8100, 1.1100, 10.0200, -0.6800, 1.3000, 10.1300, -0.0400, 1.7300, 9.0900, -0.3900, 1.1600, 9.7200, -0.9300, -0.1100, 9.5900, -1.3400, -1.4800, 9.5600, -2.0000, -2.0600, 9.7800, -2.0000, -2.0600, 9.7800, -1.7800, -1.2500, 9.8700, -0.8000, -0.4000, 9.0900, -0.5800, -0.6700, 9.9600, -0.8500, -1.0500, 9.7900, -1.4100, -2.4600, 10.0000, -1.2200, -2.4800, 9.5500, -1.2200, -2.4800, 9.5500, -0.5200, -1.2600, 9.7800, 0.2900, 0.4500, 9.6900, 0.6000, -0.6200, 9.5500, 0.2800, -1.7000, 9.2200, -0.2200, -2.3500, 10.2400, -0.2200, -2.3500, 10.2400, -0.3300, -2.0300, 10.1500, -0.1900, -1.3300, 8.9100, 0.1500, -1.5400, 9.9700, 0.1700, -1.3300, 10.3800, 0.0800, -0.7900, 9.5800, -0.3400, -1.2100, 10.3800, -0.3400, -1.2100, 10.3800, 0.1100, -1.2700, 8.6800, 0.1300, 0.6400, 9.7000, -0.1700, 1.0700, 9.4800, -0.6300, 0.2300, 10.0700, -0.7200, -0.3400, 10.0400, -0.8900, 2.3100, 10.6300, -0.8900, 2.3100, 10.6300, -0.9300, 3.2300, 10.3100, 0.5500, 1.5500, 7.4600, -0.1100, 2.8200, 9.4600, -2.0300, 1.7300, 10.4800, -0.4600, 2.3100, 10.0100, -0.5600, 3.5100, 10.2600, -0.5600, 3.5100, 10.2600, -0.8500, 2.9900, 9.8200, -1.3400, 2.4000, 10.0600, -0.4700, 0.7100, 9.9900, -1.1000, -1.0400, 9.6800, -2.0300, -2.1200, 10.2300, -3.1000, -3.0500, 9.7700, -3.1000, -3.0500, 9.7700, -0.5300, -1.6000, 9.8000, 0.5800, -0.7300, 10.0100, 0.4000, -0.5900, 9.9800
};

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

int main() {
    // This is needed so that output of printf is output immediately without buffering
    setvbuf(stdout, NULL, _IONBF, 0);


#if (KERNEL_VERSION_MAJOR < 3)
#ifdef CONFIG_SOC_NRF5340_CPUAPP // this comes from Zephyr
    // Switch CPU core clock to 128 MHz (only for NCS < 3.0.0)
    nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK, NRF_CLOCK_HFCLK_DIV_1);
#endif
#endif

    printk("Edge Impulse standalone inferencing (Zephyr)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        printk("The size of your 'features' array is not correct. Expected %d items, but had %u\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        return 1;
    }

    ei_impulse_result_t result = { 0 };

    while (1) {
        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = sizeof(features) / sizeof(features[0]);
        features_signal.get_data = &raw_feature_get_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
        printk("run_classifier returned: %d\n", res);

        if (res != 0) return 1;

        display_results(&ei_default_impulse, &result);
        
        k_msleep(2000);
    }
}
