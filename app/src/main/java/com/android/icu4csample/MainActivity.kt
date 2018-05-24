/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.icu4csample

import android.app.DatePickerDialog
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.DatePicker
import kotlinx.android.synthetic.main.activity_main.*
import java.util.*



class MainActivity : AppCompatActivity() {
    var year = Calendar.getInstance().get(Calendar.YEAR)
    var month = Calendar.getInstance().get(Calendar.MONTH)
    var day = Calendar.getInstance().get(Calendar.DAY_OF_MONTH)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
        updateDate(year, month, day)
    }

    private fun updateDate(year: Int, month: Int, day: Int) {
        this.year = year
        this.month  = month
        this.day = day

        english_date.text = getDateString(year, month, day, "en_US")
        arabic_date.text = getDateString(year, month, day, "ar_AE@calendar=islamic")
        hebrew_date.text = getDateString(year, month, day, "he_HE@calendar=hebrew")
        tibetan_date.text = getDateString(year, month, day, "bo_BO@calendar=buddhist")
    }

    fun showDatePickerDialog(v: View) {
        DatePickerDialog(this, DatePickerDialog.OnDateSetListener { _, year, month, day ->
            val newDate = Calendar.getInstance()
            newDate.set(year, month, day)
            updateDate(year, month, day)
        }, this.year, this.month, this.day).show()
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun getDateString(year: Int, month: Int, day: Int, locale: String): String

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("app")
        }
    }
}
