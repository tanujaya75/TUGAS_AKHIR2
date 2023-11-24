package com.example.greenhouse;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.google.android.material.card.MaterialCardView;

public class CardMenu2 extends AppCompatActivity {
    Intent I;
    MaterialCardView CardView1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_card_menu2);
        getSupportActionBar().hide();

        CardView1 = findViewById(R.id.GH1);
        CardView1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                I = new Intent(CardMenu2.this, Home.class);
                startActivity(I);
                finish();
            }
        });


    }
}