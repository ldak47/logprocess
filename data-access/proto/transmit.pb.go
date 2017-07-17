// Code generated by protoc-gen-go. DO NOT EDIT.
// source: transmit.proto

/*
Package process is a generated protocol buffer package.

It is generated from these files:
	transmit.proto

It has these top-level messages:
	MapEntry
	TransmitRequest
	TransmitResponse
	SrcidConfig
	ScanCondSeWord
	LocationInfo
	CookieConfig
	ClusterConfig
	CheckListCond
	SetCheckListConfigRequest
	SetCheckListConfigResponse
	GetCheckListConfigRequest
	GetCheckListConfigResponse
	GetLogProcessStatRequest
	GetLogProcessStatResponse
*/
package process

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion2 // please upgrade the proto package

type CondType int32

const (
	CondType_condIn        CondType = 1
	CondType_condNotIn     CondType = 2
	CondType_condIs        CondType = 3
	CondType_condExist     CondType = 4
	CondType_condNotExist  CondType = 5
	CondType_condEqual     CondType = 6
	CondType_condPartEqual CondType = 7
	CondType_condNotEqual  CondType = 8
	CondType_condLarger    CondType = 9
	CondType_condSmaller   CondType = 10
	CondType_condIsFront   CondType = 11
	CondType_condIsBehind  CondType = 12
)

var CondType_name = map[int32]string{
	1:  "condIn",
	2:  "condNotIn",
	3:  "condIs",
	4:  "condExist",
	5:  "condNotExist",
	6:  "condEqual",
	7:  "condPartEqual",
	8:  "condNotEqual",
	9:  "condLarger",
	10: "condSmaller",
	11: "condIsFront",
	12: "condIsBehind",
}
var CondType_value = map[string]int32{
	"condIn":        1,
	"condNotIn":     2,
	"condIs":        3,
	"condExist":     4,
	"condNotExist":  5,
	"condEqual":     6,
	"condPartEqual": 7,
	"condNotEqual":  8,
	"condLarger":    9,
	"condSmaller":   10,
	"condIsFront":   11,
	"condIsBehind":  12,
}

func (x CondType) Enum() *CondType {
	p := new(CondType)
	*p = x
	return p
}
func (x CondType) String() string {
	return proto.EnumName(CondType_name, int32(x))
}
func (x *CondType) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(CondType_value, data, "CondType")
	if err != nil {
		return err
	}
	*x = CondType(value)
	return nil
}
func (CondType) EnumDescriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

type TransmitResponse_Res_Status int32

const (
	TransmitResponse_SUCCEED TransmitResponse_Res_Status = 0
	TransmitResponse_FAILED  TransmitResponse_Res_Status = 1
)

var TransmitResponse_Res_Status_name = map[int32]string{
	0: "SUCCEED",
	1: "FAILED",
}
var TransmitResponse_Res_Status_value = map[string]int32{
	"SUCCEED": 0,
	"FAILED":  1,
}

func (x TransmitResponse_Res_Status) Enum() *TransmitResponse_Res_Status {
	p := new(TransmitResponse_Res_Status)
	*p = x
	return p
}
func (x TransmitResponse_Res_Status) String() string {
	return proto.EnumName(TransmitResponse_Res_Status_name, int32(x))
}
func (x *TransmitResponse_Res_Status) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(TransmitResponse_Res_Status_value, data, "TransmitResponse_Res_Status")
	if err != nil {
		return err
	}
	*x = TransmitResponse_Res_Status(value)
	return nil
}
func (TransmitResponse_Res_Status) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor0, []int{2, 0}
}

type SetCheckListConfigResponse_Res_Status int32

const (
	SetCheckListConfigResponse_SUCCEED SetCheckListConfigResponse_Res_Status = 0
	SetCheckListConfigResponse_FAILED  SetCheckListConfigResponse_Res_Status = 1
)

var SetCheckListConfigResponse_Res_Status_name = map[int32]string{
	0: "SUCCEED",
	1: "FAILED",
}
var SetCheckListConfigResponse_Res_Status_value = map[string]int32{
	"SUCCEED": 0,
	"FAILED":  1,
}

func (x SetCheckListConfigResponse_Res_Status) Enum() *SetCheckListConfigResponse_Res_Status {
	p := new(SetCheckListConfigResponse_Res_Status)
	*p = x
	return p
}
func (x SetCheckListConfigResponse_Res_Status) String() string {
	return proto.EnumName(SetCheckListConfigResponse_Res_Status_name, int32(x))
}
func (x *SetCheckListConfigResponse_Res_Status) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(SetCheckListConfigResponse_Res_Status_value, data, "SetCheckListConfigResponse_Res_Status")
	if err != nil {
		return err
	}
	*x = SetCheckListConfigResponse_Res_Status(value)
	return nil
}
func (SetCheckListConfigResponse_Res_Status) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor0, []int{10, 0}
}

type GetCheckListConfigResponse_Res_Status int32

const (
	GetCheckListConfigResponse_SUCCEED GetCheckListConfigResponse_Res_Status = 0
	GetCheckListConfigResponse_FAILED  GetCheckListConfigResponse_Res_Status = 1
)

var GetCheckListConfigResponse_Res_Status_name = map[int32]string{
	0: "SUCCEED",
	1: "FAILED",
}
var GetCheckListConfigResponse_Res_Status_value = map[string]int32{
	"SUCCEED": 0,
	"FAILED":  1,
}

func (x GetCheckListConfigResponse_Res_Status) Enum() *GetCheckListConfigResponse_Res_Status {
	p := new(GetCheckListConfigResponse_Res_Status)
	*p = x
	return p
}
func (x GetCheckListConfigResponse_Res_Status) String() string {
	return proto.EnumName(GetCheckListConfigResponse_Res_Status_name, int32(x))
}
func (x *GetCheckListConfigResponse_Res_Status) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(GetCheckListConfigResponse_Res_Status_value, data, "GetCheckListConfigResponse_Res_Status")
	if err != nil {
		return err
	}
	*x = GetCheckListConfigResponse_Res_Status(value)
	return nil
}
func (GetCheckListConfigResponse_Res_Status) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor0, []int{12, 0}
}

type GetLogProcessStatResponse_Res_Status int32

const (
	GetLogProcessStatResponse_SUCCEED GetLogProcessStatResponse_Res_Status = 0
	GetLogProcessStatResponse_FAILED  GetLogProcessStatResponse_Res_Status = 1
)

var GetLogProcessStatResponse_Res_Status_name = map[int32]string{
	0: "SUCCEED",
	1: "FAILED",
}
var GetLogProcessStatResponse_Res_Status_value = map[string]int32{
	"SUCCEED": 0,
	"FAILED":  1,
}

func (x GetLogProcessStatResponse_Res_Status) Enum() *GetLogProcessStatResponse_Res_Status {
	p := new(GetLogProcessStatResponse_Res_Status)
	*p = x
	return p
}
func (x GetLogProcessStatResponse_Res_Status) String() string {
	return proto.EnumName(GetLogProcessStatResponse_Res_Status_name, int32(x))
}
func (x *GetLogProcessStatResponse_Res_Status) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(GetLogProcessStatResponse_Res_Status_value, data, "GetLogProcessStatResponse_Res_Status")
	if err != nil {
		return err
	}
	*x = GetLogProcessStatResponse_Res_Status(value)
	return nil
}
func (GetLogProcessStatResponse_Res_Status) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor0, []int{14, 0}
}

type MapEntry struct {
	Key              *string `protobuf:"bytes,1,req,name=key" json:"key,omitempty"`
	Value            *string `protobuf:"bytes,2,req,name=value" json:"value,omitempty"`
	XXX_unrecognized []byte  `json:"-"`
}

func (m *MapEntry) Reset()                    { *m = MapEntry{} }
func (m *MapEntry) String() string            { return proto.CompactTextString(m) }
func (*MapEntry) ProtoMessage()               {}
func (*MapEntry) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

func (m *MapEntry) GetKey() string {
	if m != nil && m.Key != nil {
		return *m.Key
	}
	return ""
}

func (m *MapEntry) GetValue() string {
	if m != nil && m.Value != nil {
		return *m.Value
	}
	return ""
}

type TransmitRequest struct {
	Values           []*MapEntry `protobuf:"bytes,1,rep,name=values" json:"values,omitempty"`
	XXX_unrecognized []byte      `json:"-"`
}

func (m *TransmitRequest) Reset()                    { *m = TransmitRequest{} }
func (m *TransmitRequest) String() string            { return proto.CompactTextString(m) }
func (*TransmitRequest) ProtoMessage()               {}
func (*TransmitRequest) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{1} }

func (m *TransmitRequest) GetValues() []*MapEntry {
	if m != nil {
		return m.Values
	}
	return nil
}

type TransmitResponse struct {
	Status           *TransmitResponse_Res_Status `protobuf:"varint,1,req,name=status,enum=process.TransmitResponse_Res_Status" json:"status,omitempty"`
	Error            *string                      `protobuf:"bytes,2,opt,name=error" json:"error,omitempty"`
	XXX_unrecognized []byte                       `json:"-"`
}

func (m *TransmitResponse) Reset()                    { *m = TransmitResponse{} }
func (m *TransmitResponse) String() string            { return proto.CompactTextString(m) }
func (*TransmitResponse) ProtoMessage()               {}
func (*TransmitResponse) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{2} }

func (m *TransmitResponse) GetStatus() TransmitResponse_Res_Status {
	if m != nil && m.Status != nil {
		return *m.Status
	}
	return TransmitResponse_SUCCEED
}

func (m *TransmitResponse) GetError() string {
	if m != nil && m.Error != nil {
		return *m.Error
	}
	return ""
}

type SrcidConfig struct {
	Srcid            *string   `protobuf:"bytes,1,req,name=srcid" json:"srcid,omitempty"`
	Cond             *CondType `protobuf:"varint,2,req,name=cond,enum=process.CondType" json:"cond,omitempty"`
	XXX_unrecognized []byte    `json:"-"`
}

func (m *SrcidConfig) Reset()                    { *m = SrcidConfig{} }
func (m *SrcidConfig) String() string            { return proto.CompactTextString(m) }
func (*SrcidConfig) ProtoMessage()               {}
func (*SrcidConfig) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{3} }

func (m *SrcidConfig) GetSrcid() string {
	if m != nil && m.Srcid != nil {
		return *m.Srcid
	}
	return ""
}

func (m *SrcidConfig) GetCond() CondType {
	if m != nil && m.Cond != nil {
		return *m.Cond
	}
	return CondType_condIn
}

type ScanCondSeWord struct {
	Searchword       *string   `protobuf:"bytes,1,req,name=searchword" json:"searchword,omitempty"`
	Cond             *CondType `protobuf:"varint,2,req,name=cond,enum=process.CondType" json:"cond,omitempty"`
	XXX_unrecognized []byte    `json:"-"`
}

func (m *ScanCondSeWord) Reset()                    { *m = ScanCondSeWord{} }
func (m *ScanCondSeWord) String() string            { return proto.CompactTextString(m) }
func (*ScanCondSeWord) ProtoMessage()               {}
func (*ScanCondSeWord) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{4} }

func (m *ScanCondSeWord) GetSearchword() string {
	if m != nil && m.Searchword != nil {
		return *m.Searchword
	}
	return ""
}

func (m *ScanCondSeWord) GetCond() CondType {
	if m != nil && m.Cond != nil {
		return *m.Cond
	}
	return CondType_condIn
}

type LocationInfo struct {
	Province         *string   `protobuf:"bytes,1,opt,name=province" json:"province,omitempty"`
	City             *string   `protobuf:"bytes,2,opt,name=city" json:"city,omitempty"`
	Cond             *CondType `protobuf:"varint,3,req,name=cond,enum=process.CondType" json:"cond,omitempty"`
	XXX_unrecognized []byte    `json:"-"`
}

func (m *LocationInfo) Reset()                    { *m = LocationInfo{} }
func (m *LocationInfo) String() string            { return proto.CompactTextString(m) }
func (*LocationInfo) ProtoMessage()               {}
func (*LocationInfo) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{5} }

func (m *LocationInfo) GetProvince() string {
	if m != nil && m.Province != nil {
		return *m.Province
	}
	return ""
}

func (m *LocationInfo) GetCity() string {
	if m != nil && m.City != nil {
		return *m.City
	}
	return ""
}

func (m *LocationInfo) GetCond() CondType {
	if m != nil && m.Cond != nil {
		return *m.Cond
	}
	return CondType_condIn
}

type CookieConfig struct {
	Cookie           *string   `protobuf:"bytes,1,req,name=cookie" json:"cookie,omitempty"`
	Cond             *CondType `protobuf:"varint,2,req,name=cond,enum=process.CondType" json:"cond,omitempty"`
	XXX_unrecognized []byte    `json:"-"`
}

func (m *CookieConfig) Reset()                    { *m = CookieConfig{} }
func (m *CookieConfig) String() string            { return proto.CompactTextString(m) }
func (*CookieConfig) ProtoMessage()               {}
func (*CookieConfig) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{6} }

func (m *CookieConfig) GetCookie() string {
	if m != nil && m.Cookie != nil {
		return *m.Cookie
	}
	return ""
}

func (m *CookieConfig) GetCond() CondType {
	if m != nil && m.Cond != nil {
		return *m.Cond
	}
	return CondType_condIn
}

type ClusterConfig struct {
	Cluster          *string   `protobuf:"bytes,1,req,name=cluster" json:"cluster,omitempty"`
	Cond             *CondType `protobuf:"varint,2,req,name=cond,enum=process.CondType" json:"cond,omitempty"`
	XXX_unrecognized []byte    `json:"-"`
}

func (m *ClusterConfig) Reset()                    { *m = ClusterConfig{} }
func (m *ClusterConfig) String() string            { return proto.CompactTextString(m) }
func (*ClusterConfig) ProtoMessage()               {}
func (*ClusterConfig) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{7} }

func (m *ClusterConfig) GetCluster() string {
	if m != nil && m.Cluster != nil {
		return *m.Cluster
	}
	return ""
}

func (m *ClusterConfig) GetCond() CondType {
	if m != nil && m.Cond != nil {
		return *m.Cond
	}
	return CondType_condIn
}

type CheckListCond struct {
	Srcids           []*SrcidConfig    `protobuf:"bytes,1,rep,name=srcids" json:"srcids,omitempty"`
	Sewords          []*ScanCondSeWord `protobuf:"bytes,2,rep,name=sewords" json:"sewords,omitempty"`
	Lbs              []*LocationInfo   `protobuf:"bytes,3,rep,name=lbs" json:"lbs,omitempty"`
	Cookie           []*CookieConfig   `protobuf:"bytes,4,rep,name=cookie" json:"cookie,omitempty"`
	Cluster          []*ClusterConfig  `protobuf:"bytes,5,rep,name=cluster" json:"cluster,omitempty"`
	ResSrcid         []*SrcidConfig    `protobuf:"bytes,6,rep,name=res_srcid" json:"res_srcid,omitempty"`
	XXX_unrecognized []byte            `json:"-"`
}

func (m *CheckListCond) Reset()                    { *m = CheckListCond{} }
func (m *CheckListCond) String() string            { return proto.CompactTextString(m) }
func (*CheckListCond) ProtoMessage()               {}
func (*CheckListCond) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{8} }

func (m *CheckListCond) GetSrcids() []*SrcidConfig {
	if m != nil {
		return m.Srcids
	}
	return nil
}

func (m *CheckListCond) GetSewords() []*ScanCondSeWord {
	if m != nil {
		return m.Sewords
	}
	return nil
}

func (m *CheckListCond) GetLbs() []*LocationInfo {
	if m != nil {
		return m.Lbs
	}
	return nil
}

func (m *CheckListCond) GetCookie() []*CookieConfig {
	if m != nil {
		return m.Cookie
	}
	return nil
}

func (m *CheckListCond) GetCluster() []*ClusterConfig {
	if m != nil {
		return m.Cluster
	}
	return nil
}

func (m *CheckListCond) GetResSrcid() []*SrcidConfig {
	if m != nil {
		return m.ResSrcid
	}
	return nil
}

type SetCheckListConfigRequest struct {
	Application      *string        `protobuf:"bytes,1,req,name=application" json:"application,omitempty"`
	Cond             *CheckListCond `protobuf:"bytes,2,req,name=cond" json:"cond,omitempty"`
	XXX_unrecognized []byte         `json:"-"`
}

func (m *SetCheckListConfigRequest) Reset()                    { *m = SetCheckListConfigRequest{} }
func (m *SetCheckListConfigRequest) String() string            { return proto.CompactTextString(m) }
func (*SetCheckListConfigRequest) ProtoMessage()               {}
func (*SetCheckListConfigRequest) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{9} }

func (m *SetCheckListConfigRequest) GetApplication() string {
	if m != nil && m.Application != nil {
		return *m.Application
	}
	return ""
}

func (m *SetCheckListConfigRequest) GetCond() *CheckListCond {
	if m != nil {
		return m.Cond
	}
	return nil
}

type SetCheckListConfigResponse struct {
	Status           *SetCheckListConfigResponse_Res_Status `protobuf:"varint,1,req,name=status,enum=process.SetCheckListConfigResponse_Res_Status" json:"status,omitempty"`
	Error            *string                                `protobuf:"bytes,2,opt,name=error" json:"error,omitempty"`
	XXX_unrecognized []byte                                 `json:"-"`
}

func (m *SetCheckListConfigResponse) Reset()                    { *m = SetCheckListConfigResponse{} }
func (m *SetCheckListConfigResponse) String() string            { return proto.CompactTextString(m) }
func (*SetCheckListConfigResponse) ProtoMessage()               {}
func (*SetCheckListConfigResponse) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{10} }

func (m *SetCheckListConfigResponse) GetStatus() SetCheckListConfigResponse_Res_Status {
	if m != nil && m.Status != nil {
		return *m.Status
	}
	return SetCheckListConfigResponse_SUCCEED
}

func (m *SetCheckListConfigResponse) GetError() string {
	if m != nil && m.Error != nil {
		return *m.Error
	}
	return ""
}

type GetCheckListConfigRequest struct {
	Application      *string `protobuf:"bytes,1,req,name=application" json:"application,omitempty"`
	XXX_unrecognized []byte  `json:"-"`
}

func (m *GetCheckListConfigRequest) Reset()                    { *m = GetCheckListConfigRequest{} }
func (m *GetCheckListConfigRequest) String() string            { return proto.CompactTextString(m) }
func (*GetCheckListConfigRequest) ProtoMessage()               {}
func (*GetCheckListConfigRequest) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{11} }

func (m *GetCheckListConfigRequest) GetApplication() string {
	if m != nil && m.Application != nil {
		return *m.Application
	}
	return ""
}

type GetCheckListConfigResponse struct {
	Status           *GetCheckListConfigResponse_Res_Status `protobuf:"varint,1,req,name=status,enum=process.GetCheckListConfigResponse_Res_Status" json:"status,omitempty"`
	Error            *string                                `protobuf:"bytes,2,opt,name=error" json:"error,omitempty"`
	Cond             []*CheckListCond                       `protobuf:"bytes,3,rep,name=cond" json:"cond,omitempty"`
	XXX_unrecognized []byte                                 `json:"-"`
}

func (m *GetCheckListConfigResponse) Reset()                    { *m = GetCheckListConfigResponse{} }
func (m *GetCheckListConfigResponse) String() string            { return proto.CompactTextString(m) }
func (*GetCheckListConfigResponse) ProtoMessage()               {}
func (*GetCheckListConfigResponse) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{12} }

func (m *GetCheckListConfigResponse) GetStatus() GetCheckListConfigResponse_Res_Status {
	if m != nil && m.Status != nil {
		return *m.Status
	}
	return GetCheckListConfigResponse_SUCCEED
}

func (m *GetCheckListConfigResponse) GetError() string {
	if m != nil && m.Error != nil {
		return *m.Error
	}
	return ""
}

func (m *GetCheckListConfigResponse) GetCond() []*CheckListCond {
	if m != nil {
		return m.Cond
	}
	return nil
}

type GetLogProcessStatRequest struct {
	Application      *string        `protobuf:"bytes,1,req,name=application" json:"application,omitempty"`
	Startime         *int64         `protobuf:"varint,2,req,name=startime" json:"startime,omitempty"`
	Endtime          *int64         `protobuf:"varint,3,req,name=endtime" json:"endtime,omitempty"`
	Startindex       *int32         `protobuf:"varint,4,opt,name=startindex" json:"startindex,omitempty"`
	Endindex         *int32         `protobuf:"varint,5,opt,name=endindex" json:"endindex,omitempty"`
	Cond             *CheckListCond `protobuf:"bytes,6,opt,name=cond" json:"cond,omitempty"`
	XXX_unrecognized []byte         `json:"-"`
}

func (m *GetLogProcessStatRequest) Reset()                    { *m = GetLogProcessStatRequest{} }
func (m *GetLogProcessStatRequest) String() string            { return proto.CompactTextString(m) }
func (*GetLogProcessStatRequest) ProtoMessage()               {}
func (*GetLogProcessStatRequest) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{13} }

func (m *GetLogProcessStatRequest) GetApplication() string {
	if m != nil && m.Application != nil {
		return *m.Application
	}
	return ""
}

func (m *GetLogProcessStatRequest) GetStartime() int64 {
	if m != nil && m.Startime != nil {
		return *m.Startime
	}
	return 0
}

func (m *GetLogProcessStatRequest) GetEndtime() int64 {
	if m != nil && m.Endtime != nil {
		return *m.Endtime
	}
	return 0
}

func (m *GetLogProcessStatRequest) GetStartindex() int32 {
	if m != nil && m.Startindex != nil {
		return *m.Startindex
	}
	return 0
}

func (m *GetLogProcessStatRequest) GetEndindex() int32 {
	if m != nil && m.Endindex != nil {
		return *m.Endindex
	}
	return 0
}

func (m *GetLogProcessStatRequest) GetCond() *CheckListCond {
	if m != nil {
		return m.Cond
	}
	return nil
}

type GetLogProcessStatResponse struct {
	Status           *GetLogProcessStatResponse_Res_Status `protobuf:"varint,1,req,name=status,enum=process.GetLogProcessStatResponse_Res_Status" json:"status,omitempty"`
	Error            *string                               `protobuf:"bytes,2,opt,name=error" json:"error,omitempty"`
	Count            *int64                                `protobuf:"varint,3,req,name=count" json:"count,omitempty"`
	Iteminfos        []string                              `protobuf:"bytes,4,rep,name=iteminfos" json:"iteminfos,omitempty"`
	XXX_unrecognized []byte                                `json:"-"`
}

func (m *GetLogProcessStatResponse) Reset()                    { *m = GetLogProcessStatResponse{} }
func (m *GetLogProcessStatResponse) String() string            { return proto.CompactTextString(m) }
func (*GetLogProcessStatResponse) ProtoMessage()               {}
func (*GetLogProcessStatResponse) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{14} }

func (m *GetLogProcessStatResponse) GetStatus() GetLogProcessStatResponse_Res_Status {
	if m != nil && m.Status != nil {
		return *m.Status
	}
	return GetLogProcessStatResponse_SUCCEED
}

func (m *GetLogProcessStatResponse) GetError() string {
	if m != nil && m.Error != nil {
		return *m.Error
	}
	return ""
}

func (m *GetLogProcessStatResponse) GetCount() int64 {
	if m != nil && m.Count != nil {
		return *m.Count
	}
	return 0
}

func (m *GetLogProcessStatResponse) GetIteminfos() []string {
	if m != nil {
		return m.Iteminfos
	}
	return nil
}

func init() {
	proto.RegisterType((*MapEntry)(nil), "process.MapEntry")
	proto.RegisterType((*TransmitRequest)(nil), "process.TransmitRequest")
	proto.RegisterType((*TransmitResponse)(nil), "process.TransmitResponse")
	proto.RegisterType((*SrcidConfig)(nil), "process.SrcidConfig")
	proto.RegisterType((*ScanCondSeWord)(nil), "process.ScanCondSeWord")
	proto.RegisterType((*LocationInfo)(nil), "process.LocationInfo")
	proto.RegisterType((*CookieConfig)(nil), "process.CookieConfig")
	proto.RegisterType((*ClusterConfig)(nil), "process.ClusterConfig")
	proto.RegisterType((*CheckListCond)(nil), "process.CheckListCond")
	proto.RegisterType((*SetCheckListConfigRequest)(nil), "process.SetCheckListConfigRequest")
	proto.RegisterType((*SetCheckListConfigResponse)(nil), "process.SetCheckListConfigResponse")
	proto.RegisterType((*GetCheckListConfigRequest)(nil), "process.GetCheckListConfigRequest")
	proto.RegisterType((*GetCheckListConfigResponse)(nil), "process.GetCheckListConfigResponse")
	proto.RegisterType((*GetLogProcessStatRequest)(nil), "process.GetLogProcessStatRequest")
	proto.RegisterType((*GetLogProcessStatResponse)(nil), "process.GetLogProcessStatResponse")
	proto.RegisterEnum("process.CondType", CondType_name, CondType_value)
	proto.RegisterEnum("process.TransmitResponse_Res_Status", TransmitResponse_Res_Status_name, TransmitResponse_Res_Status_value)
	proto.RegisterEnum("process.SetCheckListConfigResponse_Res_Status", SetCheckListConfigResponse_Res_Status_name, SetCheckListConfigResponse_Res_Status_value)
	proto.RegisterEnum("process.GetCheckListConfigResponse_Res_Status", GetCheckListConfigResponse_Res_Status_name, GetCheckListConfigResponse_Res_Status_value)
	proto.RegisterEnum("process.GetLogProcessStatResponse_Res_Status", GetLogProcessStatResponse_Res_Status_name, GetLogProcessStatResponse_Res_Status_value)
}

func init() { proto.RegisterFile("transmit.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 793 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x09, 0x6e, 0x88, 0x02, 0xff, 0xa4, 0x55, 0xdd, 0x6e, 0xd3, 0x4a,
	0x10, 0x3e, 0x8e, 0x9b, 0xbf, 0x49, 0x9a, 0xba, 0x7b, 0xce, 0x81, 0xb4, 0x37, 0x50, 0xd3, 0xd2,
	0x08, 0x89, 0x08, 0x55, 0xbd, 0x2d, 0x55, 0x1b, 0xd2, 0x28, 0x52, 0x80, 0x8a, 0x14, 0x90, 0x90,
	0x10, 0x32, 0xce, 0xb6, 0xb5, 0x9a, 0x78, 0xd3, 0xdd, 0x4d, 0x69, 0x6e, 0x10, 0xcf, 0xc0, 0x03,
	0xf0, 0x10, 0x5c, 0xf1, 0x0c, 0xbc, 0x0a, 0xaf, 0xc0, 0x05, 0xb3, 0xeb, 0x9f, 0x38, 0x69, 0x13,
	0x42, 0xb9, 0x8a, 0x67, 0xfc, 0xcd, 0x97, 0xf9, 0xbe, 0x9d, 0x59, 0x43, 0x49, 0x72, 0xc7, 0x17,
	0x3d, 0x4f, 0x56, 0xfb, 0x9c, 0x49, 0x46, 0xb2, 0xf8, 0xe3, 0x52, 0x21, 0xec, 0xfb, 0x90, 0x7b,
	0xea, 0xf4, 0xeb, 0xbe, 0xe4, 0x43, 0x52, 0x00, 0xf3, 0x8c, 0x0e, 0xcb, 0xc6, 0xdd, 0x54, 0x25,
	0x4f, 0x16, 0x21, 0x7d, 0xe1, 0x74, 0x07, 0xb4, 0x9c, 0x52, 0xa1, 0xbd, 0x0d, 0x4b, 0x47, 0x21,
	0xc5, 0x0b, 0x7a, 0x3e, 0xa0, 0x42, 0x92, 0x35, 0xc8, 0x68, 0x84, 0xc0, 0x0a, 0xb3, 0x52, 0xd8,
	0x5a, 0xae, 0x86, 0xa4, 0xd5, 0x88, 0xd1, 0xfe, 0x08, 0xd6, 0xa8, 0x4a, 0xf4, 0x99, 0x2f, 0x28,
	0xd9, 0x86, 0x8c, 0x90, 0x8e, 0x1c, 0x08, 0xfd, 0x47, 0xa5, 0xad, 0xf5, 0xb8, 0x6c, 0x12, 0x5a,
	0xc5, 0x87, 0x77, 0x6d, 0x8d, 0x55, 0xed, 0x50, 0xce, 0x19, 0xc7, 0x76, 0x0c, 0x6c, 0x67, 0x03,
	0x20, 0xf1, 0xb2, 0x00, 0xd9, 0xf6, 0xcb, 0x5a, 0xad, 0x5e, 0x7f, 0x62, 0xfd, 0x43, 0x00, 0x32,
	0x07, 0x7b, 0xcd, 0x16, 0x3e, 0x1b, 0xf6, 0x0e, 0x14, 0xda, 0xdc, 0xf5, 0x3a, 0x35, 0xe6, 0x1f,
	0x7b, 0x27, 0x8a, 0x44, 0xa8, 0x30, 0x94, 0x78, 0x07, 0x16, 0x5c, 0xe6, 0x77, 0xb4, 0xc2, 0x52,
	0xa2, 0x7d, 0x44, 0x77, 0x8e, 0x86, 0x7d, 0x6a, 0xd7, 0xa1, 0xd4, 0x76, 0x1d, 0x5f, 0xc5, 0x6d,
	0xfa, 0x9a, 0xf1, 0x0e, 0x41, 0x76, 0x41, 0x1d, 0xee, 0x9e, 0x7e, 0xc0, 0x68, 0x5e, 0x9a, 0xe7,
	0x50, 0x6c, 0x31, 0xd7, 0x91, 0x1e, 0xf3, 0x9b, 0xfe, 0x31, 0x23, 0x16, 0xe4, 0x10, 0x73, 0xe1,
	0xf9, 0x2e, 0x45, 0x0a, 0x94, 0x43, 0x8a, 0x48, 0xe1, 0xc9, 0x61, 0x20, 0x2e, 0x26, 0x34, 0xa7,
	0x11, 0xee, 0x42, 0xb1, 0xc6, 0xd8, 0x99, 0x47, 0x43, 0x5d, 0x25, 0xc8, 0xb8, 0x3a, 0x9e, 0xb7,
	0xa3, 0x3d, 0x58, 0xac, 0x75, 0x07, 0x42, 0x52, 0x1e, 0x32, 0x2c, 0x41, 0xd6, 0x0d, 0x12, 0xf3,
	0x52, 0xfc, 0x34, 0x90, 0xe3, 0x94, 0xba, 0x67, 0x2d, 0x4f, 0x48, 0x95, 0x25, 0xeb, 0x78, 0xb0,
	0xca, 0xdd, 0x68, 0x1e, 0xfe, 0x8b, 0x8b, 0x92, 0x67, 0x50, 0x81, 0xac, 0xa0, 0xca, 0x3d, 0x81,
	0xdc, 0x0a, 0x76, 0x7b, 0x04, 0x1b, 0xf7, 0xda, 0x06, 0xb3, 0xfb, 0x5e, 0xa0, 0x0b, 0x0a, 0xf5,
	0x7f, 0x8c, 0x1a, 0xb3, 0x72, 0x23, 0x56, 0xbe, 0x30, 0x01, 0x1b, 0x33, 0x68, 0x73, 0x24, 0x2f,
	0xad, 0x71, 0xb7, 0x46, 0xb8, 0x31, 0x1f, 0x36, 0x21, 0xcf, 0x71, 0xae, 0x82, 0x29, 0xc9, 0x4c,
	0x97, 0x61, 0xbf, 0x82, 0x95, 0x36, 0x95, 0x49, 0x03, 0x30, 0x1b, 0x6d, 0xc6, 0xbf, 0x50, 0x70,
	0xfa, 0xfd, 0xae, 0x17, 0x34, 0x1a, 0x3a, 0xba, 0x9e, 0x70, 0x74, 0xac, 0x81, 0xa4, 0x89, 0xf6,
	0x67, 0x03, 0x56, 0xaf, 0x23, 0x0e, 0x97, 0xe7, 0xf1, 0xc4, 0xf2, 0x54, 0x47, 0xcd, 0x4d, 0x2d,
	0xfa, 0xfb, 0x35, 0x7a, 0x04, 0x2b, 0x8d, 0x3f, 0x12, 0x6b, 0x7f, 0x43, 0x19, 0x8d, 0x9b, 0xc8,
	0x68, 0xdc, 0x40, 0x46, 0x6c, 0xad, 0x39, 0x79, 0xb6, 0x63, 0xd6, 0xce, 0x29, 0xf6, 0x8b, 0x01,
	0x65, 0xec, 0xa2, 0xc5, 0x4e, 0x0e, 0x03, 0x1a, 0x55, 0x30, 0xf3, 0x64, 0x71, 0x9f, 0x51, 0x0d,
	0x97, 0x5e, 0x2f, 0xb8, 0x2d, 0x4d, 0xb5, 0x4e, 0xd4, 0xef, 0xe8, 0x84, 0xa9, 0x13, 0xea, 0xde,
	0xd0, 0x10, 0xbf, 0x43, 0x2f, 0x71, 0x56, 0x8d, 0x4a, 0x5a, 0x95, 0x21, 0x28, 0xc8, 0xa4, 0x75,
	0x26, 0xd2, 0x91, 0xc1, 0x68, 0xba, 0x8e, 0xaf, 0x86, 0x3e, 0x8e, 0xc9, 0x06, 0x43, 0x6b, 0x77,
	0x26, 0xac, 0x7d, 0x98, 0xb4, 0xf6, 0xfa, 0x9a, 0x19, 0xce, 0x62, 0xe8, 0xb2, 0x81, 0x2f, 0x43,
	0x19, 0xcb, 0x90, 0xf7, 0x24, 0xed, 0x79, 0xb8, 0x7a, 0x42, 0x6f, 0xdc, 0xbc, 0x23, 0xf4, 0xe0,
	0xbb, 0x01, 0xb9, 0xe8, 0xee, 0x50, 0x2f, 0x94, 0xce, 0xa6, 0x6f, 0x19, 0xf8, 0x0f, 0x79, 0xf5,
	0xfc, 0x8c, 0x49, 0x0c, 0x53, 0xf1, 0x2b, 0x61, 0x99, 0xd1, 0xab, 0xfa, 0x25, 0x2a, 0xb7, 0x16,
	0xd0, 0xaf, 0x62, 0x88, 0x0c, 0x32, 0xe9, 0x18, 0x70, 0x3e, 0x70, 0xba, 0x56, 0x06, 0xbb, 0x5b,
	0x54, 0xe1, 0x21, 0xfa, 0x1c, 0xa4, 0xb2, 0xc9, 0x1a, 0x9d, 0xc9, 0xe1, 0x5d, 0x09, 0x2a, 0xd3,
	0x72, 0xf8, 0x09, 0xe5, 0x56, 0x1e, 0x8f, 0xaa, 0xa0, 0xe2, 0x76, 0xcf, 0xe9, 0x76, 0x31, 0x01,
	0x51, 0xa2, 0x29, 0x0e, 0x38, 0xf3, 0xa5, 0x55, 0x88, 0x38, 0x9a, 0x62, 0x9f, 0x9e, 0xe2, 0x69,
	0x59, 0xc5, 0xad, 0x1f, 0x29, 0x28, 0x45, 0x3e, 0x52, 0x7e, 0xe1, 0xb9, 0x94, 0xec, 0x42, 0x2e,
	0xfa, 0x7c, 0x91, 0xf2, 0x35, 0x5f, 0x34, 0x3d, 0x3e, 0xab, 0x2b, 0x53, 0xbf, 0x75, 0xe4, 0x2d,
	0x90, 0xab, 0x2b, 0x4c, 0xec, 0x99, 0xfb, 0x1d, 0x90, 0xde, 0x9b, 0xe3, 0x0e, 0x50, 0xf4, 0x8d,
	0x59, 0xf4, 0x8d, 0x39, 0xe8, 0x67, 0x2c, 0xf4, 0x1b, 0x58, 0xbe, 0x32, 0x5e, 0x64, 0x6d, 0xd6,
	0xe8, 0x05, 0xe4, 0xf6, 0xef, 0xa7, 0x73, 0xdf, 0xfc, 0x64, 0x18, 0xbf, 0x02, 0x00, 0x00, 0xff,
	0xff, 0xff, 0x43, 0xb6, 0x3f, 0xc1, 0x08, 0x00, 0x00,
}