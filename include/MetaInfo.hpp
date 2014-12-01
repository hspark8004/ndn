#include <stdint.h>
#include <unistd.h>

class MetaInfo {
    private : 
        uint8_t ContentType;
        uint64_t FreshnessPeriod;
        uint64_t FinalBlockId;

    public : 
        MetaInfo() {

        }
        MetaInfo(uint8_t _ContentType, uint64_t _FreshnessPeriod, uint64_t _FinalBlockId){
            setContentType(_ContentType);
            setFreshnessPeriod(_FreshnessPeriod);
            setFinalBlockId(_FinalBlockId);
        }
        void setContentType(uint8_t ContentType) {
            this->ContentType = ContentType;
        }
        uint8_t getContentType() {
            return this->ContentType;
        }

        void setFreshnessPeriod(uint64_t FreshnessPeriod) {
            this->FreshnessPeriod = FreshnessPeriod;
        }
        uint64_t getFreshnessPeriod() {
            return this->FreshnessPeriod;
        }

        void setFinalBlockId(uint64_t FinalBlockId) {
            this->FinalBlockId = FinalBlockId;
        }
        uint64_t getFinalBlockId() {
            return this->FinalBlockId;
        }
};
