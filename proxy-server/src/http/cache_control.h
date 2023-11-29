#pragma once

class CacheControl {
    private:

    public:
        bool shouldReadCache();
        bool shouldWriteCache();
};