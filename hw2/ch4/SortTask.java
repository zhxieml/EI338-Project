package com.company;

import java.util.concurrent.RecursiveAction;

public abstract class SortTask<T extends Comparable<T>> extends RecursiveAction {
    private static final int THRESHOLD = 100;

    protected int begin;
    protected int end;
    protected T[] array;

    public SortTask(int begin, int end, T[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    @Override
    protected void compute() {
        Sort();
    }

    protected abstract void Sort();
}
